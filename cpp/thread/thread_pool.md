# ThreadPool 

- thread pool
```
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include <queue>
#include <atomic>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <future>


namespace inf {
namespace frame {
/** 
 * @class ThreadPool.
 * note:
 * ThreadPool thread_pool;
 * thread_pool.init(100); // init the thread num
 * 2 ways to start the task:
 * //1.start directly
 * thread_pool.start()
 * //2.start with a initilize function
 * thread_pool.start(func, args...);
 * 
 * run task
 * throw one task into thread_pool
 * submit(func1, args...);
 * the task will be run by one thread. return the std::future
 * return type is a std::future
 * 
 * 
 * //stop
 **/

} // end namespace frame
} // end namesapce inf


//vec function
std::vector<std::function<void()>> vecs;

template <typename F, typename ...Args>
int put(std::vector<std::function<void()>> &vecs, F&& func, Args&& ...args) {
    std::function<void()> f = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
    vecs.push_back(f);
}

int get_sum(int a, int b) {
    std::cout << a + b << std::endl;
    return a + b;
}

int get_multiply(int a, int b) {
      std::cout << a * b << std::endl;
      return a * b;
}

class TaskPool {
    using ThreadPtr = std::unique_ptr<std::thread>;
    using Functor   = std::function<void()>;
    using FunctorPtr = std::shared_ptr<Functor>;
protected:
    // timeout thread pool
    struct TaskFunc {
        int64_t     _timeout_ms;
        Functor     _func;
        TaskFunc(const int64_t timeout_ms) : _timeout_ms(timeout_ms) {};
    };
public:


    TaskPool() = default;
    
    int init(const int thread_num = 5) {
        std::unique_lock<std::mutex> lock(_lock);
        _running = false;
        _thread_num = thread_num;
        _thread_pool.reserve(_thread_num);

        for (int i = 0; i < _thread_num; ++i) {
            ThreadPtr thread_ptr(new std::thread());
            _thread_pool.emplace_back(std::move(thread_ptr));
        }
        
        return 0;
    }

    template <typename F, typename ...Args>
    auto enqueue(F&& func, Args&& ...args) -> std::future<decltype(func(args...))>
    {
        using RetType = decltype(func(args...));
        //change parameter functor to no parameter functor clusore, then packaged_task
        auto task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(func), std::forward<Args>(args)...));
        std::unique_lock<std::mutex> lock(_lock);
        _task_queue.push(task);
        _cond.notify_one();
        return task->get_future();
    }

    FunctorPtr get_task() {
        std::unique_lock<std::mutex> lock(_lock);
        if (_task_queue.empty()) {
            _cond.wait(lock);
        }
            auto task = _task_queue.front();
            _task_queue.pop();
        return task;
    }


    int run() {
        while (_running) {
            auto task = get_task();
            (*task)();
        }
    }

    int start() {
        std::unique_lock<std::mutex> lock(_lock);
        _running = true;
        for (auto &ths : _thread_pool) {
            *ths = std::thread(run, this);
        }
    }

    int stop() {
        std::unique_lock<std::mutex> lock(_lock);
        _running = false;
        _cond.notify_all();

        for (auto &ths : _thread_pool) {
            if (ths->joinable()) {
                ths->join();
            }
            //all ths is shared_ptr, no need to manually delete
        }
    }

private:

    
    /* task queue. */
    std::queue<FunctorPtr>              _task_queue;

    std::atomic<bool>                _running;

    std::vector<ThreadPtr>           _thread_pool;

    std::mutex                       _lock;
    
    std::condition_variable          _cond;

    int64_t                          _thread_num;

};

int main() {

    put(vecs, get_sum, 10, 10);
    put(vecs, get_multiply, 10, 10);

    auto func = [](int a, int b) { 
        std::cout << a % b << std::endl;
        return a % b;
        };
    put(vecs, func, 10, 10);

    put(vecs, [](int a, int b, int c) {
       std::cout << a + b + c << std::endl;
       return a + b + c;
    }, 
        10, 10, 10);
   

    for (auto &f : vecs) {
        f();
    }

    return 0;
}
```


```
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <atomic>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <unistd.h>
#include <queue>
#include <future>

namespace inf {
namespace frame {
const int64_t DEFAULT_THREAD_NUM = 5;
/** 
 * @class ThreadPool.
 * note:
 * ThreadPool thread_pool;
 * thread_pool.init(100); // init the thread num
 * 2 ways to start the task:
 * //1.start directly
 * thread_pool.start()
 * //2.start with a initilize function
 * thread_pool.start(func, args...);
 * 
 * run task
 * throw one task into thread_pool
 * submit(func1, args...);
 * the task will be run by one thread. return the std::future
 * return type is a std::future
 * 
 * 
 * //stop
 **/
class ThreadPool {
protected:
    struct FuncTask {
        int64_t                 _timeout_ms;//task run timeout time
        std::function<void()>   _func;      //func
        
        FuncTask(const int timeout_ms = 0) : _timeout_ms(timeout_ms) {};
    };
public:

    using FuncPtr = std::shared_ptr<FuncTask>;
    using ThreadPtr = std::unique_ptr<std::thread>;
    /* ctor. */
    ThreadPool() = default;
    virtual ~ThreadPool() = default;

    /**
    * init the trhead pool
    * @param thread_num
    * @return 0 if ok. otherwise failed
    */
    int init(const int thread_num = DEFAULT_THREAD_NUM) {
        std::unique_lock<std::mutex> lock(_lock);
        _thread_num = thread_num;
       
        for (int64_t i = 0; i < _thread_num; ++i) {
            ThreadPtr thread_ptr(new std::thread());
            _thread_pool.push_back(std::move(thread_ptr));
        }

        return 0;
    }

    /**
    * submit task in to the task queue.
    * you can submit a callable obj such as lambda funciton, function and functor.
    * and will return a std::future<ReturnType> after the callable obj be executed.
    * e.g.:
    * auto result = submit(100, func, a, b, c);
    * result.get() will blocking untill the task has been executed.
    * @param func and args...
    * @param timeout_ms task timeout ms
    * @return std::future<>
    */
    template <typename Func, typename ...Args>
    auto submit(int64_t timeout_ms, Func&& func, Args&& ...args) -> std::future<decltype(func(args...))>
    {   
        using ReturnType = decltype(func(args...));
        auto task = std::make_shared<std::packaged_task<ReturnType()>>
                        (std::bind(func, std::forward<Args>(args)...));

        auto f_ptr = std::make_shared<FuncTask>(timeout_ms);
        f_ptr->_func = [task]{
            //execute the packaged task
            (*task)();
        };

        std::unique_lock<std::mutex> lock(_lock);
        _task_queue.push(f_ptr);
        _cond.notify_one();

        return task->get_future();
    }

    /**
    * submit task in to the task queue.
    * you can submit a callable obj such as lambda funciton, function and functor.
    * and will return a std::future<ReturnType> after the callable obj be executed.
    * e.g.:
    * auto result = submit(func, a, b, c);
    * result.get() will blocking untill the task has been executed.
    * @param func and args...
    * @param timeout_ms task timeout ms
    * @return std::future<>
    */
    template <typename Func, typename ...Args>
    auto submit(Func&& func, Args&& ...args) -> std::future<decltype(func(args...))> {
        return submit(0, func, args...);
    }


    /**
    * get task from the task queue.
    * blocked when the queue is empty.
    * @return FuncPtr
    */
   FuncPtr get_task() {
       std::unique_lock<std::mutex> lock(_lock);
       if (_task_queue.empty()) {
           _cond.wait(lock);
       }

       auto task_ptr = std::move(_task_queue.front());
       _task_queue.pop();
       
       return task_ptr;
   }

    /**
    * run the thread pool, thread call here to get task from the queue. 
    */
   void run() {
       while(_is_running) {
           ++_task_num;
           auto task = get_task();
           if (task != nullptr) {
                task->_func();
           }
           --_task_num;
           
           //time out 
           //    if (task->_timeout_ms != 0 && )
           if (_task_num == 0 && _task_queue.empty()) {
               _cond.notify_all();
           }
       }
   }

    /**
    * start the task pool
    * @return 0 if ok , otherwise failed
    */
    int start() {
        std::unique_lock<std::mutex> lock(_lock);
        _is_running = true;
        for (auto& ths : _thread_pool) {
            *ths = std::thread(&::inf::frame::ThreadPool::run, this);
        }
        return 0;
    }

    /**
    * stop thread pool
    * @return 0 if ok, otherwise failed.
    */
   int stop() {
       {
       std::unique_lock<std::mutex> lock(_lock);
       _is_running = false;
       _cond.notify_all();
       }

       for (auto &ths : _thread_pool) {
           if (ths->joinable()) {
               ths->join();
           }
       }
       return 0;
   }

    /**
    * get queued task num.
    * @return task num
    */
   int64_t get_queued_task_num() {
       std::unique_lock<std::mutex> lock(_lock);
       return _task_queue.size();
   }

    /**
    * get running task num.
    * @return running task num 
    */
   int64_t get_running_task_num() {
       return _task_num;
   }





private:
    /* none copy. */
    ThreadPool(const ThreadPool &lhs) = delete;
    ThreadPool &operator=(const ThreadPool &lhs) = delete;

    /* thread pool. */
    std::vector<ThreadPtr>                  _thread_pool;

    /* thread num. */
    int                                     _thread_num;

    /* task queue. */
    std::queue<FuncPtr>                     _task_queue;

    /* mutex lock. */
    std::mutex                              _lock;
    
    /* cond. */
    std::condition_variable                 _cond;

    /* running task num. */
    std::atomic<int>                        _task_num{0};

    /* running status. */
    std::atomic<bool>                       _is_running{false};

    /* timeout ms. */
    int64_t                                 _timeout_ms_default{0};

};





} // end namespace frame
} // end namespace inf


int main() {
    std::cout << "hello world" << std::endl;

    ::inf::frame::ThreadPool pool;
    pool.init(10);
    pool.start();

    std::function<int(int, int)> f = [](int a, int b) {return a + b;};
     
    auto task = pool.submit(f, 10, 10);
    auto task2 = pool.submit(f, 20, 30);

    std::cout << task.get() << std::endl;
    std::cout << task2.get() << std::endl;


    sleep(10);
    pool.stop();
    return 0;

}
```