#include <thread>
#include <iostream>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <vector>
#include <queue>
#include <unistd.h>
#include <future>
#include <atomic>
#include <sys/syscall.h>

#include "threadpool.hpp"

namespace std{
class ThreadPool{
public:
    typedef function<void()> Task;
    ThreadPool(int size){
        for(int i=0;i<size;i++){
            pool.emplace_back([this](){
                while(_run){
                    Task task;
                    {
                        unique_lock<mutex>lk(mu);
                        cond.wait(lk, [this] {
                            return !_run || !tasks.empty();
                            });  // wait 直到有 task
                        if (!_run && tasks.empty()) return;
                        task = move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }
    ~ThreadPool(){
        _run = false;
        cond.notify_all();
        for(auto& thread:pool){
            if(thread.joinable()){
                thread.join();
            }
        }
    }

    // template<class F,class ...Args>
    // auto commit(F&&f, Args&&...args) -> std::future<decltype(f(args...))>{
    //     using ret_type = decltype(f(args...));
    //     auto task = make_shared<packaged_task<ret_type()>>(bind(forward<F>(f),forward<Args>(args)...));
    //     future<ret_type> future=task->get_future();
    //     {
    //         lock_guard<mutex>lk(mu);
    //         tasks.emplace([task](){
    //             (*task)();
    //         });
    //     }
    //     cond.notify_one();
    //     return future;
    // }

    auto commit(Task task){
        {
            lock_guard<mutex>lk(mu);
            tasks.emplace([task](){
                task();
            });
        }
        cond.notify_one();
    }

private:
    int _size;
    mutex mu;
    condition_variable cond;
    vector<thread>pool;
    queue<Task>tasks;
    std::atomic<bool> _run{true};
};
};

void f(){
    int i=10;
    while(i--) {
        sleep(1);
        std::cout << i << std::endl;
    }
}

void g(){
    int i=10;
    while(i--){
        sleep(1);
        std::cout << "world" << std::endl;
    }
}

int main(){
    std::ThreadPool pool(3);
    pool.commit(f);
    pool.commit(g);
    return 0;
}