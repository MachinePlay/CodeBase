#ifndef _UTILS_DOUBLE_BUFFER_H
#define _UTILS_DOUBLE_BUFFER_H

#include <memory>
#include <atomic>
#include <thread>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unordered_map>

#include "yaml-cpp/yaml.h"
#include "log/common_log.h"

namespace {

const uint32_t          BUF_SIZE = 2;
const uint32_t          DEFAULT_MONITOR_INTERVAL = 3;
const uint32_t          DEFAULT_OLD_BUF_LIFE_TIME = 30;
const std::string       NULLPTR_FILE = "";

}

namespace yrs {
namespace util {

class SwitchMonitor {
public:
    /*constructor*/
    SwitchMonitor() : _last_modify_time(0) {}

    /*destructor*/
    ~SwitchMonitor() {}

    /*init SwitchMonitor*/
    bool init(const YAML::Node &conf) {
        try {
            _update_file = conf["update_file"].as<std::string>();
            _done_file = conf["done_file"].as<std::string>();
            
            return _attach();
        } catch (const std::exception &e) {
            ERRLOG << e.what() << std::endl;
            return false;
        } catch (...) {
            ERRLOG << "occur unknown exception" << std::endl;
            return false;
        }
    }

    /*check whether should switch double buffer*/
    bool should_switch_monitor() {
        if (!_create()) {
            return false;
        }

        struct stat buf;
        if (stat(_update_file.c_str(), &buf) != 0) {
            ERRLOG << "stat error : " << _update_file << std::endl;
            return false;
        }

        if (buf.st_mtime > _last_modify_time) {
            _last_modify_time = buf.st_mtime;
            return true;    
        }

        return false;
    }

    /*dump switch done flag*/
    void switch_done(bool flag) {
        std::ofstream done_file_handler(_done_file.c_str(), 
                std::ios::out | std::ios::trunc);
        if (done_file_handler.is_open()) {
            done_file_handler.put(flag == true ? '1' : '0');
            done_file_handler.close();
        }
    }

private:
    SwitchMonitor(const SwitchMonitor &rhs);
    SwitchMonitor& operator=(const SwitchMonitor &rhs);

    /*attach file monitor*/
    bool _attach() {
        //notice firstly create empty update file
        if (!_create()) {
            ERRLOG << "_create() update file failed.\n";
            return false;
        }

        //init _last_modify_time of update file
        struct stat buf;
        if (stat(_update_file.c_str(), &buf) != 0) {
            ERRLOG << "stat error : " << _update_file << std::endl;
            return false;
        }
         
        _last_modify_time = buf.st_mtime;

        return true;
    }

    /*create update flag file*/
    bool _create() const {
        if (access(_update_file.c_str(), F_OK) != 0) {
            return open(_update_file.c_str(), 
                    O_RDWR | O_CREAT) != -1 ? true : false;
        }
        
        return true;
    }

    /*update file last modify time*/
    std::time_t _last_modify_time;

    /*update flag file*/
    std::string _update_file;

    /*update done flag file*/
    std::string _done_file;
};

template <typename Buffer, typename Loader>
class DoubleBuffer {
private:
    typedef std::unique_ptr<Loader>      LoaderPtr;
    typedef std::unique_ptr<Buffer>      BufferPtr;
    typedef std::unique_ptr<std::thread> ThreadPtr;

public:
    /*constructor*/
    DoubleBuffer(LoaderPtr loader) : 
            _loader(std::move(loader)),
            _cur_idx(0),
            _stop_monitor(false),
            _monitor_interval(DEFAULT_MONITOR_INTERVAL),
            _old_buf_life_time(DEFAULT_OLD_BUF_LIFE_TIME) {}

    /*destructor notice stop monitor thread*/
    ~DoubleBuffer() {
        _stop_monitor = true;
        if (_monitor_thread) {
            _monitor_thread->join();
        }
    }

    /*init DoubleBuffer create actual buffer data and create monitor thread*/
    bool init(const YAML::Node &conf) {
        try {
            if (!_loader) {
                ERRLOG << "_loader is nullptr.\n";
                return false;
            }

            BufferPtr buffer = _loader->load();
            if (!buffer) {
                ERRLOG << "_loader->load() is nullptr.\n ";
                return false;
            }

            _double_buffer[_cur_idx] = std::move(buffer);
            _monitor_interval = conf["monitor_interval"].as<uint32_t>();
            _old_buf_life_time = conf["old_buf_life_time"].as<uint32_t>();

            if (!_switch_monitor.init(conf["switch_monitor"])) {
                return false;
            }
            
            //create monitor thread 
            _monitor_thread.reset(new std::thread(_monitor_update, this));
        } catch (const std::exception &e) {
            ERRLOG << e.what() << std::endl;
            return false;
        } catch (...) {
            ERRLOG << "occur unknown exception" << std::endl;
            return false;
        }

        return true;
    }

    const BufferPtr& get_buffer() const {
        return _double_buffer[_cur_idx];
    }
    
private:
    /*none-copy*/
    DoubleBuffer(const DoubleBuffer &rhs);
    DoubleBuffer& operator=(const DoubleBuffer &rhs);

    /*monitor callback*/
    static void _monitor_update(DoubleBuffer *bufs) {
        if (bufs == nullptr) {
            ERRLOG << "callback param is nullptr.\n";
            return;
        }

        while (!bufs->_stop_monitor) {
            sleep(bufs->_monitor_interval);

            //check whether should switch buffer
            if (!bufs->_switch_monitor.should_switch_monitor()) {
                continue;
            }

            uint32_t unused_idx = 1 - bufs->_cur_idx;
            
            //clear old buffer before loading
            bufs->_double_buffer[unused_idx] = nullptr;

            //load new buffer data
            BufferPtr buffer = bufs->_loader->load();
            if (!buffer) {
                bufs->_switch_monitor.switch_done(false);
                ERRLOG << "_loader->load() is nullptr.\n";
                continue;
            }
            bufs->_double_buffer[unused_idx] = std::move(buffer);

            //switch used buffer index
            bufs->_cur_idx = unused_idx;

            //touch switch success flag
            bufs->_switch_monitor.switch_done(true);

            //clear old buffer if needed.
            if (bufs->_old_buf_life_time > 0) {
                sleep(bufs->_old_buf_life_time);
                bufs->_double_buffer[1 - bufs->_cur_idx] = nullptr;
            }
        }
    }

    /*switch monitor*/
    SwitchMonitor _switch_monitor;

    /*double buffer instance*/
    BufferPtr _double_buffer[BUF_SIZE];

    /*buffer loader instance*/
    LoaderPtr _loader;

    /*monitor thread instance*/
    ThreadPtr _monitor_thread;

    /*current used buffer index*/
    std::atomic<uint32_t> _cur_idx;

    /*monitor stop flag*/
    std::atomic<bool> _stop_monitor;

    /*monitor interval (in seconds) */
    uint32_t _monitor_interval;

    /*old buffer life time*/
    uint32_t _old_buf_life_time;
};

class DoubleBufferConfigureManager {
public:
    /* singleton */
    static DoubleBufferConfigureManager& instance() {
        static DoubleBufferConfigureManager configure_manager;
        return configure_manager;
    }

    /* Destructor*/
    ~DoubleBufferConfigureManager() {}

    /* load double buffer configure */
    bool init(const std::string &conf_path) {
        try {
            YAML::Node conf = YAML::LoadFile(conf_path.c_str());
            for (uint32_t i = 0; i < conf.size(); ++i) {
                _configure_node_table.insert(std::make_pair(
                        conf[i]["command_key"].as<std::string>(), conf[i]));
            }

            return true;
        } catch (const std::exception &e) {
            ERRLOG << e.what() << "|" << conf_path << std::endl;
            return false;
        } catch (...) {
            ERRLOG << "unknown exception|" << conf_path << std::endl;
            return false;
        }
    }

    /*get yaml configure node */
    const YAML::Node& get_configure_node(const std::string &key) const {
        const auto &iter = _configure_node_table.find(key);
        if (iter == _configure_node_table.end()) {
            return _nullptr_node;
        }

        return iter->second;
    }

    /*get monitor file using for taf-command callback */
    std::string get_monitor_file(const std::string &key) const {
        try {
            const auto &iter = _configure_node_table.find(key);
            if (iter == _configure_node_table.end()) {
                return NULLPTR_FILE;
            }

            return iter->second["switch_monitor"]["update_file"].as<std::string>();
        } catch (const std::exception &e) {
            ERRLOG << e.what() << "|" << key << std::endl;
            return NULLPTR_FILE;
        } catch (...) {
            ERRLOG << "unknown exception|" << key << std::endl;
            return NULLPTR_FILE;
        }
    }

private:
    /* Constructor */
    DoubleBufferConfigureManager() {}

    /* configure node typedef */
    typedef std::unordered_map<std::string, YAML::Node>  HashConfigureNode;
    
    /* configure node nullptr */
    YAML::Node _nullptr_node;
    
    /* configure hash */
    HashConfigureNode _configure_node_table;
};

} //end namespace util
} //end namespace yrs

#endif // end _UTILS_DOUBLE_BUFFER_H
