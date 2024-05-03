/**
 * @file child_scheduler.hpp
 * @author Yonah Goldberg (ygoldber@andrew.cmu.edu)
 * @author Jack Ellinger (jellinge@andrew.cmu.edu)
 * 
 * @brief A child stealing scheduler. Each thread has its own deque that is protected
 * via a fine-grained lock. Spawned tasks are pushed to the front of a thread's deque. If
 * a thread has no work, it steals from the back of another thread's deque. Threads only steal
 * from their own queue while synchronizing (waiting on dependencies).
 * 
 */

#ifndef CHILD_SCHEDULER_LF_HPP
#define CHILD_SCHEDULER_LF_HPP

#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <deque>
#include <thread>
#include <vector>

#include "scheduler.hpp"
#include "lock-free-queue/TaskQueue.hpp"
#include "lock-free-queue/Task.hpp"

template <typename T> class ChildSchedulerLF : public Scheduler<T> {
private:
  // A task that a thread can run.
//   struct Task {
//     std::packaged_task<T()> func;
//   };
  // All the threads in the thread pool
  std::vector<std::thread> threads;
  // Map from std::thread::id to an integer thread id that is easier to work
  // with.
  std::unordered_map<std::thread::id, int> threadIds;
  // Each thread has an associated queue of tasks for it to run.
  std::vector<TaskQueue<T> *> taskQueues;
  // The number of threads in thread pool
  int n;
  // Number of tasks across all queues
  std::atomic<int> taskCount = 0;
  // Number of threads currently doing work
  std::atomic<int> workCount = 0;

public:
  ChildSchedulerLF() {}

  // Create a thread pool of size n, put func into main thread's task queue,
  // and call workerThread. This function returns when all work is done and
  // all threads are joined.
  T run(std::function<T()> func, int n) {
    this->n = n;
    taskQueues.resize(n);
    taskCount = 1;

    for (int i = 1; i < n; i++) {
      // emplace_back efficiently stores the thread without needing an extra
      // move
      TaskQueue<T> *queue = new TaskQueue<T>();
      taskQueues.push_back(queue);
      threads.emplace_back(&ChildSchedulerLF::workerThread, this, i);
      threadIds[threads[i].get_id()] = i;
    }

    threadIds[std::this_thread::get_id()] = 0;
    std::packaged_task<T()> task(func);
    auto fut = task.get_future();
    taskQueues[0]->Push(new Task<T>{std::move(task)});

    workerThread(0);

    // join threads when finished
    for (auto& t : threads) {
      t.join();
    }

    threads.clear();
    threadIds.clear();

    // Return result of func if there is one 
    if constexpr (std::is_void<T>::value) {
      fut.get();
    } else {
      return fut.get();
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
      taskQueues[tid]->Push(new Task<T>{std::move(task)});
    }

    taskCount.fetch_add(1, std::memory_order_relaxed);
    return std::move(fut);
  }

  TaskQueue<T>* GetRandomTaskQueue()
  {
    // printf("GET RANDOM QUEUE\n");
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distribution(1, n);
    size_t index = static_cast<size_t> ( std::round( distribution( gen ) ) );
    // printf("%d\n", index);
    TaskQueue<T>* queue = taskQueues[index - 1];
    // printf("GET RANDOM QUEUE FINISHED\n");
    return queue;
  }

  Task<T>* getTask(int curTid) {
    // printf("GET TASK \n");
    TaskQueue<T>* queue = taskQueues[curTid];
    Task<T>* task = queue->Pop();
    // printf("GET TASK POPPED\n");
    if (task == nullptr){
      TaskQueue<T> *randomQueue = GetRandomTaskQueue();
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
  T sync(std::future<T> fut) {
    int tid = getTid();

    // While future is not valid, attempt to steal work
    while (fut.wait_for(std::chrono::milliseconds(0)) !=
           std::future_status::ready) {
      Task<T>* task;
      bool foundTask = false;
      {
        task = getTask(curTid);
        if (task != nullptr) {
            foundTask = true;
        }
        // std::unique_lock<std::mutex> lock(locks[tid]);
        // if (!taskQueues[tid].empty()) {
        //   foundTask = true;
        //   task = std::move(taskQueues[tid].front());
        //   taskQueues[tid].pop_front();
        // }
      }

      if (foundTask) {
        taskCount.fetch_sub(1, std::memory_order_relaxed);
      } else {
        continue;
      }

      // There is a task to run. Execute it!
      task->func();
    }

    // Return result of future if there is one
    if constexpr (std::is_void<T>::value) {
      fut.get();
    } else {
      return fut.get();
    }
  }

private:
  // Get the callling threads integer thread ID
  int getTid() { return threadIds[std::this_thread::get_id()]; }

  void workerThread(int tid) {
    int curTid = tid;

    // Loop continuously over all the work queues, starting with this thread's
    // queue If we find any work to do, pop the work off and complete it! This
    // naive way of finding work might cause a lot of contention!
    while (true) {
      Task<T>* task;
      bool foundTask = false;
      {
        task = getTask(curTid);
        if (task != nullptr) {
            foundTask = true;
        }
        // if (!taskQueues[curTid].empty()) {
        //   foundTask = true;
        //   if (curTid == tid) {
        //     task = std::move(taskQueues[curTid].front());
        //     taskQueues[curTid].pop_front();
        //   } else {
        //     task = std::move(taskQueues[curTid].back());
        //     taskQueues[curTid].pop_back();
        //   }
        // }
      }

      if (foundTask) {
        workCount.fetch_add(1, std::memory_order_relaxed);
        taskCount.fetch_sub(1, std::memory_order_relaxed);
      } else {
        // No more tasks across all queues AND no workers currently running a task
        // If a workers is running a task then it might add more tasks to its queue,
        // so we keep this thread runing
        if (taskCount == 0 && workCount == 0) {
          break;
        }

        // Keep this thread running and check next queue
        continue;
      }

      // There is a task to run. Execute it!
      task->func();
      workCount.fetch_sub(1, std::memory_order_relaxed);
    }
  }
};

#endif