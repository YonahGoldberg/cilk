#ifndef SIMPLE_CS_SCHEDULER_HPP
#define SIMPLE_CS_SCHEDULER_HPP

#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "scheduler.hpp"

template <typename T> class SimpleCSScheduler : public Scheduler<T> {
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
  // Each thread has an associated queue of tasks for it to run.
  std::vector<std::queue<Task>> taskQueues;
  // Each task queue has an associated mutex for accessing.
  std::vector<std::mutex> locks;
  // The number of threads in thread pool
  int n;
  // Set to true when we want all threads to exit
  bool finish = false;

public:
  SimpleCSScheduler() {}

  // Initialize the child stealing scheduler with a thread pool size of n
  void init(int n) {
    this->n = n;
    taskQueues.resize(n);
    std::vector<std::mutex> muts(n);
    locks.swap(muts);

    for (int i = 0; i < n; i++) {
      // emplace_back efficiently stores the thread without needing an extra
      // move
      threads.emplace_back(&SimpleCSScheduler::workerThread, this, i);
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
      std::cout << "locking 1" << std::endl;
      std::unique_lock<std::mutex> lock(locks[tid]);
      taskQueues[tid].emplace(Task{std::move(task)});
    }
    std::cout << "unlocking 1" << std::endl;
    return fut;
  }

  // Attempt to steal work while waiting on fut to finish
  T steal(std::future<T> fut) {
    int curTid = getTid();

    // While future is not valid, attempt to steal work
    while (!fut.valid()) {
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
    if constexpr (!std::is_void<T>::value) {
      return fut.get();
    }
  }

  ~SimpleCSScheduler() {
    // Set finish to true to signal all threads to exit their main routine
    finish = true;
    for (auto &thread : threads) {
      std::cout << "Waiting for join" << std::endl;
      thread.join();
      std::cout << "Got join" << std::endl;
    }
  }

private:
  // Get the callling threads integer thread ID
  int getTid() { return threadIds[std::this_thread::get_id()]; }

  void workerThread(int tid) {
    int curTid = getTid();

    // Loop continuously over all the work queues, starting with this thread's
    // queue If we find any work to do, pop the work off and complete it! This
    // naive way of finding work might cause a lot of contention!
    while (!finish) {
      Task task;
      bool foundTask = false;
      {
        std::cout << "Locking 2" << std::endl;
        std::unique_lock<std::mutex> lock(locks[curTid]);
        if (!taskQueues[curTid].empty()) {
          foundTask = true;
          task = std::move(taskQueues[curTid].front());
          taskQueues[curTid].pop();
        }
      }
      std::cout << "Unlocking 2" << std::endl;

      if (!foundTask) {
        // No task in current queue, try next queue
        curTid = (curTid + 1) % n;
        std::cout << "no task" << std::endl;
        continue;
      }

      // There is a task to run. Execute it!
      task.func();
    }
  }
};

#endif