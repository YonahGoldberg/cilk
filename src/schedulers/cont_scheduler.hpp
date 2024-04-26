#ifndef CONT_SCHEDULER_HPP
#define CONT_SCHEDULER_HPP

#include <csetjmp>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <deque>
#include <thread>
#include <unordered_map>
#include <vector>

#include "scheduler.hpp"

const size_t STACK_SIZE = 16 * 1024;

enum TaskType {
  CREATED,
  CONTINUATION,
};

template <typename T> struct Task {
  std::packaged_task<T()> func;
  void *stackBottom;
  void *stackTop;
  TaskType type;
  jmp_buf *buf;

  Task() : func(std::packaged_task<T()>([] { return 0; })) {}
  Task(std::packaged_task<T()> task)
      : func(std::move(task)), stackBottom(new char[STACK_SIZE]),
        stackTop(static_cast<char *>(stackBottom) + STACK_SIZE),
        type(TaskType::CREATED), buf((jmp_buf *)malloc(sizeof(jmp_buf))) {}

  Task(Task &&rhs) {
    stackBottom = std::exchange(rhs.stackBottom, nullptr);
    stackTop = rhs.stackTop;
    type = rhs.type;
    buf = rhs.buf;
    func = std::move(rhs.func);
  }

  Task &operator=(Task &&rhs) {
    Task copy(std::move(rhs));
    copy.swap(*this);
    return *this;
  }

  ~Task() {
    if (stackBottom != nullptr) {
      delete[] static_cast<char *>(stackBottom);
      free(buf);
    }
  }

  void swap(Task &rhs) {
    std::swap(func, rhs.func);
    std::swap(stackBottom, rhs.stackBottom);
    std::swap(stackTop, rhs.stackTop);
    std::swap(type, rhs.type);
    std::swap(buf, rhs.buf);
  }
};

template <typename T> class SimpleContScheduler : public Scheduler<T> {
private:
  void schedule() {
    while (true) {
      {
        std::unique_lock lock(mut);
        // All tasks completed, safe to exit
        // std::cout << tasks.size() << " tasks in stack" << std::endl;
        if (tasks.size() == 0 && taskCount == 0)
          return;
        // Wait on more work to be produced
        else if (tasks.size() == 0 && taskCount != 0)
          continue;
        // There is work to grab!
        // std::cout << "start" << std::endl;
        curTask = std::move(tasks.front());
        // std::cout << "end" << std::endl;
        tasks.pop_front();
        // std::cout << "grabbed work!" << std::endl;
        // Increment taskCount so other threads don't exit if stack is now empty
        taskCount++;
      }

      if (curTask.type == TaskType::CREATED) {
        void *top = curTask.stackTop;
        asm volatile("mov sp, %[rs] \n" : : [rs] "r"(top) : "memory");

        curTask.func();
        // std::cout << "finished func" << std::endl;
        {
          std::unique_lock lock(mut);
          // Done with task
          taskCount--;
        }
        longjmp(schedulerBuf, 1);
      } else {
        // std::cout << "jumping to continuation" << std::endl;
        longjmp(*curTask.buf, 1);
      }
    }
  }

  void workerThread() {
    setjmp(schedulerBuf);
    // std::cout << "workerThread" << std::endl;
    schedule();
  }

public:
  static std::deque<Task<T>> tasks;
  static std::mutex mut;
  // All the threads in the thread pool
  std::vector<std::thread> threads;
  // Number of tasks currently being run
  static int taskCount;
  thread_local static jmp_buf schedulerBuf;
  thread_local static Task<T> curTask;

  SimpleContScheduler() {}

  T run(std::function<T()> func, int n) {
    std::packaged_task<T()> task(func);
    auto fut = task.get_future();
    tasks.emplace_front(std::move(Task(std::move(task))));
    for (int i = 0; i < n; i++) {
      threads.emplace_back(&SimpleContScheduler::workerThread, this);
    }

    // Join all worker threads to wait for them to finish
    for (auto& thread : threads) {
        thread.join();
    }

    threads.clear();
    return fut.get();
  }

  std::future<T> spawn(std::function<T()> func) {
    std::packaged_task<T()> task(func);
    auto fut = task.get_future();

    if (setjmp(*curTask.buf) == 0) {
      // task currently running can be continued later
      curTask.type = TaskType::CONTINUATION;
      // add this continuation task and the new task to stack
      {
        std::unique_lock lock(mut);
        taskCount--;
        tasks.emplace_front(std::move(curTask));
        tasks.emplace_front(std::move(Task(std::move(task))));
      }
      // longjmp to scheduler to start working on the next task
      longjmp(schedulerBuf, 1);
    }

    // If running continuation, fall through and return to task
    return fut;    
  }

  T sync(std::future<T> fut) {
    {
      std::unique_lock lock(mut);
      taskCount--;
    }
    longjmp(schedulerBuf, 1);
    return 0;
  }
};

template <typename T> std::deque<Task<T>> SimpleContScheduler<T>::tasks;

template <typename T> std::mutex SimpleContScheduler<T>::mut;

template <typename T> thread_local jmp_buf SimpleContScheduler<T>::schedulerBuf;

template <typename T> thread_local Task<T> SimpleContScheduler<T>::curTask;

template <typename T> int SimpleContScheduler<T>::taskCount;

#endif