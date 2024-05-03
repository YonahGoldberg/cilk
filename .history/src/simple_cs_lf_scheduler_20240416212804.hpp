#ifndef SIMPLE_CS_LF_SCHEDULER_HPP
#define SIMPLE_CS_LF_SCHEDULER_HPP

#include <functional>
#include <future>
#include <iostream>
#include <stdatomic.h>
#include "lock-free-queue/JobQueue.hpp"
#include "lock-free-queue/Task.h"
#include <queue>
#include <thread>
#include <vector>

#include "scheduler.hpp"

template <typename T> class SimpleCSLFScheduler : public Scheduler<T> {
private:
  // A task that a thread can run.
  // struct Task {
  //   std::packaged_task<T()> func;
  // };
  // All the threads in the thread pool
  std::vector<std::thread> threads;
  // Map from std::thread::id to an integer thread id that is easier to work
  // with.
  std::unordered_map<std::thread::id, int> threadIds;
  // Each thread has an associated queue of tasks for it to run.
  std::vector<JobQueue<T> *> taskQueues; // task queue for each worker
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
    taskQueues.reserve(n);

    // Add the rest of the workers
    for (size_t i = 0; i < n; i++)
    {
      JobQueue<T> *queue = new JobQueue<T>(100);
      taskQueues.push_back(queue);
      threads.emplace_back(&SimpleCSLFcheduler::workerThread, this, i);
      threadIds[threads[i].get_id()] = i;
    }
  }

  // Spawn new function to potentially be run in parallel.
  // This function gets stored on this thread's task queue and can be stolen
  // later by this thread, or another thread if another thread runs out of work.
  std::future<T> spawn(std::function<T()> func) {
    std::packaged_task<T()> task(func);
    int tid = getTid();
    auto fut = task.get_future();

    {
      // Lock current thread's task queue before accessing
      taskQueues[tid]->Push(Task{std::move(task)});
    }
    return fut;
  }

  JobQueue<T>* GetRandomJobQueue()
  {
      static std::random_device rd;
      static std::mt19937 gen(rd());
      static std::uniform_int_distribution<> distribution(0, n);

    size_t index = static_cast<size_t> ( std::round( distribution( gen ) ) );
    return taskQueues[index];
  }

  Task* getTask(int curTid) {
    JobQueue<T>* queue = taskQueues[curTid];
    Task<T>* task = queue->Pop();
    if (task == nullptr){
      JobQueue<T> *randomQueue = GetRandomJobQueue();
      if (randomQueue == nullptr)
      {
        std::this_thread::yield();
        return nullptr;
      }

      if ( queue == randomQueue )
      {
        std::this_thread::yield();
        return nullptr;
      }
      task = randomQueue->Steal();
      if (task == nullptr)
      {
        std::this_thread::yield();
        return nullptr;
      }
    }
    return task;
  }

  // Attempt to steal work while waiting on fut to finish
  T steal(std::future<T> fut) {
    int curTid = getTid();

    // While future is not valid, attempt to steal work
    while (fut.wait_for(std::chrono::milliseconds(0)) !=
           std::future_status::ready) {
      Task<T>* task = getTask(curTid);
      if (task != nullptr) {
        task->func();
        // might need to do parent dependencies and stuff
      }
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
    finish = false;
  }

private:
  // Get the callling threads integer thread ID
  int getTid() { return threadIdToWorker[std::this_thread::get_id()]; }

  void workerThread(int tid) {
    int curTid = getTid();

    while (!finish) {
      Task<T>* task = getTask(curTid);
      if (task != nullptr) {
        task->func();
        // might need to do parent dependencies and stuff
      }
    }
  }
};

#endif