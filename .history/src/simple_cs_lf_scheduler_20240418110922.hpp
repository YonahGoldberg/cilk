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
      threads.emplace_back(&SimpleCSLFScheduler::workerThread, this, i);
      threadIds[threads[i].get_id()] = i;
    }
    // printf("GOT PAST INIT\n");
  }

  // Spawn new function to potentially be run in parallel.
  // This function gets stored on this thread's task queue and can be stolen
  // later by this thread, or another thread if another thread runs out of work.
  std::future<T> spawn(std::function<T()> func) {
    // printf("STARTED SPAWN\n");
    std::packaged_task<T()> task(func);
    int tid = getTid();
    auto fut = task.get_future();
    // printf("GOT FUTURE\n");
    {
      // Lock current thread's task queue before accessing
      // printf("START PUSH IN SPAWN\n");
      taskQueues[tid]->Push(new Task<T>{std::move(task)});
      // printf("PUSHED IN SPAWN\n");
    }
    return fut;
  }

  JobQueue<T>* GetRandomJobQueue()
  {
    // printf("GET RANDOM QUEUE\n");
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distribution(1, n);
    size_t index = static_cast<size_t> ( std::round( distribution( gen ) ) );
    // printf("%d\n", index);
    JobQueue<T>* queue = taskQueues[index - 1];
    // printf("GET RANDOM QUEUE FINISHED\n");
    return queue;
  }

  Task<T>* getTask(int curTid) {
    // printf("GET TASK \n");
    JobQueue<T>* queue = taskQueues[curTid];
    Task<T>* task = queue->Pop();
    // printf("GET TASK POPPED\n");
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
    // printf("STEAL START\n");
    int curTid = getTid();

    // While future is not valid, attempt to steal work
    while (fut.wait_for(std::chrono::milliseconds(0)) !=
           std::future_status::ready) {
      Task<T>* task = getTask(curTid);
      if (task != nullptr) {
        // task->func();
        // might need to do parent dependencies and stuff
        try {
          task->func();  // Execute the task
        } catch (const std::exception& e) {
          printf("ERROR ALERT");
          // Handle the exception or continue
        }
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
  int getTid() { return threadIds[std::this_thread::get_id()]; }

  void workerThread(int tid) {
    int curTid = getTid();

    while (!finish) {
      Task<T>* task = getTask(curTid);
      if (task != nullptr) {
        // task->func();
        try {
          task->func();  // Execute the task
        } catch (const std::exception& e) {
          printf("%d\n", curTid);
          // Handle the exception or continue
        }
        // might need to do parent dependencies and stuff
      }
    }
  }
};

#endif