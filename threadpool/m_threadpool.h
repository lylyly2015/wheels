#pragma once
#include <vector>
#include <mutex>
#include <future>
#include <functional>
#include <queue>
#include <atomic>
#include <iostream>
#include <unistd.h>
#include <sys/syscall.h>

#define THREADPOOL_MAXNUM 16

class ThreadPool{
public:
    inline ThreadPool(size_t size = 4) {
        addThread(size);
    }
    inline ~ThreadPool(){
        _run = false;
        _cond.notify_all();
        for(auto &thread : _pool){
            //std::cout<<"destroy thread ID: " << thread.get_id() << std::endl;
            if(thread.joinable()) {
                std::cout<<"destroy thread ID: " << thread.get_id() << std::endl;
                thread.join(); 
            }
        }
    }
private:
    inline int get_freeThreadNum(){
        return _freeThreadNum;
    }
    inline int get_allThreadNum(){
        return _pool.size();
    }
    template<class F,class ...Args>
    auto commit(F&&f, Args&&...args) -> std::future<decltype(f(args...))>{
        using RetType = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<RetType()>>
            (std::bind(std::forward<F>(f),std::forward<Args>(args)...));
        std::future<RetType> future = task->get_future();
        {
            std::lock_guard<std::mutex>lk(_lock);
            _tasks.emplace(*task);
        }
        _cond.notify_one();
        return future;
    }

    void addThread(size_t size){
        for(; _pool.size() < THREADPOOL_MAXNUM && size > 0; size--){
            _pool.emplace_back([this]{
                while(_run){
                    Task task;
                    {
                        std::unique_lock<std::mutex>lk(_lock);
                        std::cout << "创建第 " << _pool.size() << "条线程,ID = " << syscall(SYS_gettid) << std::endl;
                        _cond.wait(lk,[this]{
                            return !_run || !_tasks.empty();
                        });
                        if(!_run && _tasks.empty()){
                            return ;
                        }
                        task = std::move(_tasks.front());
                        _tasks.pop();
                    }
                    _freeThreadNum--;
                    task();
                    _freeThreadNum++;
                }
            });
            _freeThreadNum++;
        }
    }
private:
    using Task = std::function<void()>;
    std::vector<std::thread> _pool;
    std::queue<Task>_tasks;
    std::mutex _lock;
    std::condition_variable _cond;
    std::atomic<bool> _run{true};
    std::atomic<int> _freeThreadNum{0};
};