#pragma once
#include <mutex>

template<size_t blocksize,size_t blocknum=10>
class MemoryPool{
public:
    MemoryPool(){
        std::lock_guard<std::mutex>lk(freeblock_mu);
        freeblock_head=nullptr;
        memchunk_head=nullptr;
    }

    ~MemoryPool(){
        std::lock_guard<std::mutex>lk(freeblock_mu);
        memchunk* p;
        while(memchunk_head){
            p = memchunk_head->next;
            delete p;
            memchunk_head = p;
        }
    }

    void* allocate(){
        std::lock_guard<std::mutex>lk(freeblock_mu);
        if(!freeblock_head){
            // 申请一大块新的内存区
            memchunk* new_chunk = new memchunk();
            new_chunk->next = nullptr;
            freeblock_head = &(new_chunk->blocks[0]);
            // 把这一大块内存区中的小内存块串起来
            for (int i = 1; i < blocknum; i++) {
                new_chunk->blocks[i-1].next = &(new_chunk->blocks[i]);
            }
            new_chunk->blocks[blocknum-1].next = nullptr;
            // 更新内存区指针
            if (!memchunk_head) {
                memchunk_head = new_chunk;
            } else {
                memchunk_head->next = new_chunk;
                memchunk_head = new_chunk;
            }
        }
        void* object_block = freeblock_head;
        freeblock_head = freeblock_head->next;
        return object_block;
    }

    void* allocate(size_t size){
        std::lock_guard<std::mutex>lk(memchunk_mu);
        int n = size / blocksize;
        void* p = allocate();
        for (int i = 1; i < n; i++) {
            allocate();
        }
        return p;
    }

    void deallocate(void* p){
        std::lock_guard<std::mutex>lk(freeblock_mu);
        freeblock* block = static_cast<freeblock*>(p);
        block->next = freeblock_head;
        freeblock_head = block;
    }

private:
    struct freeblock{
        unsigned char data[blocksize];
        freeblock*next;
    };
    freeblock* freeblock_head;
    
    struct memchunk{
        freeblock blocks[blocknum];
        memchunk*next;
    };
    memchunk* memchunk_head;

    std::mutex freeblock_mu; // 内存块指针
    std::mutex memchunk_mu; // 内存区指针
};