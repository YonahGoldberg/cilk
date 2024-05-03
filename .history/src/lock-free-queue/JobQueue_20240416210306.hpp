#pragma once

#include <atomic>
#include <vector>
#include <algorithm>

// Assuming Task is defined elsewhere, such as in "Task.h"
// #include "Task.h"

template <typename T>
class JobQueue {
public:
    JobQueue(size_t maxJobs) 
        : maxJobs ( maxJobs )
    {
        bottomIndex = 0;
        topIndex = 0;
        queue.resize( maxJobs );
    }

    void Push(Task<T> *job) {
        int bottom = bottomIndex.load(std::memory_order_seq_cst);
        queue[bottom] = job;
        bottomIndex.store(bottom + 1, std::memory_order_seq_cst);
    }

    Task<T>* Pop() {
        int bottom = bottomIndex.load( std::memory_order_seq_cst );
        bottom = std::max ( 0, bottom - 1 );
        bottomIndex.store( bottom, std::memory_order_seq_cst );
        int top = topIndex.load( std::memory_order_seq_cst);

        if ( top <= bottom )
        {
            Task *job = queue[ bottom ];

            // There are several jobs in the queue, we don't need to worry about Steal()
            if ( top != bottom )
            {
                return job;
            }

            // This is the last item in the queue, we need to check if a Steal() has increased top
            int stolenTop = top + 1;
            if( topIndex.compare_exchange_strong( stolenTop, top + 1, std::memory_order_seq_cst ) )
            {
                // An Steal() call has stolen our job (https://www.youtube.com/watch?v=DEiWU1MbBfk)
                bottomIndex.store( stolenTop, std::memory_order_release );
                return nullptr;
            }
            return job;
        }
        else
        {
            bottomIndex.store( top, std::memory_order_seq_cst );
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
    std::vector<Task*> queue;
};
