#pragma once

#include <atomic>
#include <vector>
#include <algorithm>
#include "Task.hpp"


template <typename T>
class TaskQueue {
public:
    TaskQueue()
    {
        bottomIndex = 0;
        topIndex = 0;
        queue.resize(10000);
    }

    void Push(Task<T> *job, int tid) {
        print
        int bottom = bottomIndex.load(std::memory_order_seq_cst);
        queue[bottom] = job;
        bottomIndex.store(bottom + 1, std::memory_order_seq_cst);
    }

    Task<T>* Pop(int curTid) {
        printf("BEGAN POP for %d \n", curTid);
        printf("FR\n");
        int bottom = bottomIndex.load(std::memory_order_seq_cst);
        printf("BEGAN loaded bottom for %d \n", curTid);
        bottom = std::max (0, bottom - 1);
        bottomIndex.store(bottom, std::memory_order_seq_cst);
        int top = topIndex.load(std::memory_order_seq_cst);
        printf("LOADED TOP for %d \n", curTid);
        if (top <= bottom)
        {
            printf("ACCESSING BOTTOM %d \n", curTid);
            Task<T> *job = queue[ bottom ];
            printf("ACCESSED BOTTOM %d \n", curTid);
            if ( top != bottom )
            {
                return job;
            }
            int stolenTop = top + 1;
            if( topIndex.compare_exchange_strong(stolenTop, top + 1, std::memory_order_seq_cst))
            {
                bottomIndex.store(stolenTop, std::memory_order_release);
                return nullptr;
            }
            return job;
        }
        else
        {
            bottomIndex.store(top, std::memory_order_seq_cst);
            return nullptr;
        }
    }

    Task<T>* Steal() {
        int top = topIndex.load(std::memory_order_seq_cst);
        int bottom = bottomIndex.load(std::memory_order_seq_cst);

        if (top < bottom) {
            Task<T> *job = queue[top];
            if (topIndex.compare_exchange_strong(top, top + 1, std::memory_order_seq_cst)) {
                return job;
            }
            return nullptr;
        } else {
            return nullptr;
        }
    }

    size_t Size() const {
        return static_cast<size_t>(bottomIndex - topIndex);
    }

    void Clear() {
        bottomIndex = 0;
        topIndex = 0;
    }

private:
    int maxJobs;
    std::atomic_int bottomIndex;
    std::atomic_int topIndex;
    std::vector<Task<T>*> queue;
};