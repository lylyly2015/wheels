#pragma once
#include <mutex>
#include <thread>

template <class T>
class shared_ptr2{
public:
    shared_ptr2(T* ptr = nullptr)
        :_pPtr(ptr)
        ,_refcount(new int(1))
        ,_pmutex(new std::mutex)
    {}
    ~shared_ptr2(){
        Release();
    }
    shared_ptr2(shared_ptr2<T>& sp)
        :_pPtr(sp._pPtr)
        ,_refcount(sp._refcount)
        ,_pmutex(sp._pmutex)
    {
        Addrefcount();
    }
    shared_ptr2<int> & operator = (const shared_ptr2<T>& sp){
        if(_pPtr != nullptr){
            Release();
            _pPtr = sp._pPtr;
            _refcount = sp._refcount;
            _pmutex = sp._pmutex;
            Addrefcount();
        }
        return *this;
    }
    T* operator ->(){
        return _pPtr;
    }
    T& operator *(){
        return *_pPtr;
    }
    int Usecount(){
        return *_refcount;
    }
    T*get(){
        return _pPtr;
    }
    void Addrefcount(){
        _pmutex->lock();
        ++(*_refcount);
        _pmutex->unlock();
    }
private:
    void Release(){
        bool deleteflag = false;
        if(*_refcount == 1){
            delete _pPtr;
            delete _refcount;
            deleteflag = true;
        }
        if(deleteflag == true){
            delete _pmutex;
        }
    }
    int * _pPtr;
    int * _refcount;
    std::mutex*_pmutex;
};