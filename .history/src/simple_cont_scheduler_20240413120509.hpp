#ifndef SIMPLE_CONT_SCHEDULER_HPP
#define SIMPLE_CONT_SCHEDULER_HPP

#include <csetjmp>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

#include "scheduler.hpp"

template <typename T>
class SimpleContScheduler : public Scheduler<T> {
private:
    // A task that a thread can run.
    struct Task {
        std::packaged_task<T()> func;
        jmp_buf context;
        bool isContextSet = false;
    };

    // All the threads in the thread pool
    std::vector<std::thread> threads;
    // Map from std::thread::id to an integer thread id that is easier to work with
    std::unordered_map<std::thread::id, int> threadIds;
    // Each thread has an associated queue of tasks for it to run
    std::vector<std::queue<Task>> taskQueues;
    // Each task queue has an associated mutex for accessing
    std::vector<std::mutex> locks;
    // The number of threads in the thread pool
    int n;
    // Set to true when we want all threads to exit
    bool finish = false;

public:
    SimpleContScheduler() {}

    void init(int n) {
        this->n = n;
        taskQueues.resize(n);
        std::vector<std::mutex> muts(n);
        locks.swap(muts);
        for (int i = 0; i < n; ++i) {
            threads.emplace_back(&SimpleContScheduler::workerThread, this, i);
            threadIds[threads[i].get_id()] = i;
        }
    }

    std::future<T> spawn(std::function<T()> func) {
        std::packaged_task<T()> task(func);
        int tid = getTid();
        auto fut = task.get_future();
        std::unique_lock<std::mutex> lock(locks[tid]);
        taskQueues[tid].emplace(Task{std::move(task)});
        return fut;
    }

    T steal(std::future<T> fut) {
        int curTid = getTid();
        while (fut.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
            Task task;
            bool foundTask = false;
            for (int i = 0; i < n; ++i) {
                int tid = (curTid + i) % n;
                std::unique_lock<std::mutex> lock(locks[tid]);
                if (!taskQueues[tid].empty()) {
                    task = std::move(taskQueues[tid].front());
                    taskQueues[tid].pop();
                    foundTask = true;
                    break;
                }
            }

            if (foundTask) {
                if (!task.isContextSet) {
                    if (setjmp(task.context) == 0) {
                        task.func();
                    }
                } else {
                    longjmp(task.context, 1);
                }
            } else {
                std::this_thread::yield(); // Yield thread if no task found
            }
        }

        return fut.get();
    }

    void cleanup() {
        finish = true;
        for (auto &thread : threads) {
            thread.join();
        }
        threads.clear();
        threadIds.clear();
        taskQueues.clear();
        locks.clear();
        finish = false;
    }

private:
    int getTid() {
        return threadIds[std::this_thread::get_id()];
    }

    void workerThread(int tid) {
        while (!finish) {
            Task task;
            bool foundTask = false;
            {
                std::unique_lock<std::mutex> lock(locks[tid]);
                if (!taskQueues[tid].empty()) {
                    task = std::move(taskQueues[tid].front());
                    taskQueues[tid].pop();
                    foundTask = true;
                }
            }

            if (foundTask) {
                if (!task.isContextSet) {
                    if (setjmp(task.context) == 0) {
                        task.func();
                    }
                } else {
                    longjmp(task.context, 1);
                }
            } else {
                std::this_thread::yield(); // Yield thread if no task found
            }
        }
    }
};

#endif // SIMPLE_CONT_SCHEDULER_HPP
