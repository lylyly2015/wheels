#pragma once
#include <mutex>

class RW_lock{
public:
    void read_lock(){
        r_mutex.lock();
        if(++read_cnt == 1){
            w_mutex.lock();
        }
        r_mutex.unlock();
    }

    void read_unlock(){
        r_mutex.lock();
        if(--read_cnt == 0){
            w_mutex.unlock();
        }
        r_mutex.unlock();
    }

    void write_lock(){
        w_mutex.lock();
    }

    void write_unlock(){
        w_mutex.unlock();
    }
    
private:
    std::mutex w_mutex;
    std::mutex r_mutex;
    size_t read_cnt;
};