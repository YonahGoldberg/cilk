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
        queue.resize(100000);
    }

    void Push(Task<T> *job, int tid) {
        // printf("DID PUSH FOR %d \n", tid);
        int bottom = bottomIndex.load(std::memory_order_seq_cst);
        // printf("GOT BOTTOM FOR %d \n", tid);
        queue[bottom] = job;
        bottomIndex.store(bottom + 1, std::memory_order_seq_cst);
        // printf("UPDATED BOTTOM %d \n", tid);
    }

    Task<T>* Pop(int curTid) {
        int bottom = bottomIndex.load(std::memory_order_seq_cst);
        bottom = std::max (0, bottom - 1);
        bottomIndex.store(bottom, std::memory_order_seq_cst);
        bottom = bottomIndex.load(std::memory_order_seq_cst);
        int top = topIndex.load(std::memory_order_seq_cst);
        if (top <= bottom)
        {
            Task<T> *job = std::move(queue[bottom]);
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

    Task<T>* Steal(int curTid, size_t randomIndex) {
        // printf("BEGAN STEAL for %d, %d\n", curTid, randomIndex);
        int top = topIndex.load(std::memory_order_seq_cst);
        int bottom = bottomIndex.load(std::memory_order_seq_cst);
        // printf("DID LOAD for %d, %d\n", curTid, randomIndex);
        if (top < bottom) {
            Task<T> *job = std::move(queue[top]);
            // printf("MOVED JOB\n");
            if (topIndex.compare_exchange_strong(top, top + 1, std::memory_order_seq_cst)) {
                // printf("RETURNED JOB\n");
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