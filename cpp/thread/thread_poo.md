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