#ifndef SIMPLE_CS_LF_SCHEDULER_HPP
#define SIMPLE_CS_LF_SCHEDULER_HPP

#include <functional>
#include <future>
#include <iostream>
#include <stdatomic.h>
#include "lock-free-queue/JobQueue.h"
#include "lock-free-queue/Worker.h"
#include <queue>
#include <thread>
#include <vector>

#include "scheduler.hpp"

template <typename T> class SimpleCSLFScheduler : public Scheduler<T> {
private:
  // A task that a thread can run.
  struct Task {
    std::packaged_task<T()> func;
  };
  // All the threads in the thread pool
  std::vector<std::thread> threads;
  // Map from std::thread::id to an integer thread id that is easier to work
  // with.
  std::unordered_map<std::thread::id, int> threadIds;
  // std::unordered_map<std::thread::id, Worker> threadIdToWorker;
  // Each thread has an associated queue of tasks for it to run.
  // std::vector<std::queue<Task>> taskQueues;
  std::vector<JobQueue *> taskQueues; // task queue for each worker
  std::vector<Worker *> workers; // worker 
  // Each task queue has an associated mutex for accessing.
  // std::vector<int8_t> locks;
  int n; // The number of threads in thread pool
  // Set to true when we want all threads to exit
  bool finish = false;

public:
  SimpleCSLFScheduler() {}

  // Initialize the child stealing scheduler with a thread pool size of n
  void init(int n) {
    this->n = n;
    workers.reserve(n);
    taskQueues.reserve(n);

    // JobQueue *queue = new JobQueue(100); // need to adjust size
	  // queues.push_back( queue );
	  // Worker *mainThreadWorker = new Worker( this, queue );
	  // workers.push_back( mainThreadWorker );

    // Add the rest of the workers
    for (size_t i = 0; i < n; i++)
    {
      JobQueue *queue = new JobQueue(100);
      taskQueues.push_back(queue);
      threads.emplace_back(&SimpleCSLFcheduler::workerThread, this, i);
      threadIds[threads[i].get_id()] = i;
      // Worker *worker = new Worker(this, queue );
      // workers.push_back( worker );
    }
  }

  // Spawn new function to potentially be run in parallel.
  // This function gets stored on this thread's task queue and can be stolen
  // later by this thread, or another thread if another thread runs out of work.
  std::future<T> spawn(std::function<T()> func) {
    std::packaged_task<T()> task(func);
    int worker = getWorker();
    auto fut = task.get_future();

    {
      // Lock current thread's task queue before accessing
      // std::unique_lock<std::mutex> lock(locks[tid]);

      taskQueues[tid].emplace(Task{std::move(task)});
    }
    return fut;
  }

  // Attempt to steal work while waiting on fut to finish
  T steal(std::future<T> fut) {
    int curTid = getTid();

    // While future is not valid, attempt to steal work
    while (fut.wait_for(std::chrono::milliseconds(0)) !=
           std::future_status::ready) {
      Task task;
      bool foundTask = false;
      {

        std::unique_lock<std::mutex> lock(locks[curTid]);
        if (!taskQueues[curTid].empty()) {
          foundTask = true;
          task = std::move(taskQueues[curTid].front());
          taskQueues[curTid].pop();
        }
      }

      if (!foundTask) {
        // No task in current queue, try next queue
        curTid = (curTid + 1) % n;
        continue;
      }

      // There is a task to run. Execute it!
      task.func();
    }

    // Return result of future if there is one
    if constexpr (std::is_void<T>::value) {
      fut.get();
    } else {
      return fut.get();
    }
  }

  void cleanup() {
    // Set finish to true to signal all threads to exit their main routine
    finish = true;
    for (auto &thread : threads) {
      thread.join();
    }

    // clear state from previous iteration
    threads.clear();
    threadIds.clear();
    taskQueues.clear();
    locks.clear();
    finish = false;
  }

private:
  // Get the callling threads integer thread ID
  Worker getWorker() { return threadIdToWorker[std::this_thread::get_id()]; }

  void workerThread(int tid) {
    int curTid = getTid();

    // Loop continuously over all the work queues, starting with this thread's
    // queue If we find any work to do, pop the work off and complete it! This
    // naive way of finding work might cause a lot of contention!
    while (!finish) {
      Task task;
      bool foundTask = false;
      {
        std::unique_lock<std::mutex> lock(locks[curTid]);
        if (!taskQueues[curTid].empty()) {
          foundTask = true;
          task = std::move(taskQueues[curTid].front());
          taskQueues[curTid].pop();
        }
      }

      if (!foundTask) {
        // No task in current queue, try next queue
        curTid = (curTid + 1) % n;
        continue;
      }

      // There is a task to run. Execute it!
      task.func();
    }
  }
};

#endif