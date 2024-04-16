#ifndef SIMPLE_CONT_SCHEDULER_HPP
#define SIMPLE_CONT_SCHEDULER_HPP

#include <csetjmp>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <stack>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_map>

const size_t STACK_SIZE = 16 * 1024;

enum TaskType {
  CREATED,
  CONTINUATION,
};

template <typename T> struct Task {
  std::function<T()> func;
  void *stackBottom;
  void *stackTop;
  TaskType type;
  jmp_buf *buf = (jmp_buf *)malloc(sizeof(jmp_buf));

  Task() : func([] {}) {}

  Task(std::function<T()> func)
      : func(std::move(func)), stackBottom(new char[STACK_SIZE]),
        stackTop(static_cast<char *>(stackBottom) + STACK_SIZE),
        type(TaskType::CREATED) {}

  Task(Task &&rhs) {
    func = rhs.func;
    stackBottom = std::exchange(rhs.stackBottom, nullptr);
    stackTop = rhs.stackTop;
    type = rhs.type;
    buf = rhs.buf;
  }

  Task &operator=(Task &&rhs) {
    Task copy(std::move(rhs));
    copy.swap(*this);
    return *this;
  }

  ~Task() {
    if (stackBottom != nullptr) {
      delete[] static_cast<char *>(stackBottom);
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

template <typename T> class SimpleContScheduler {
private:
  void schedule() {
    if (tasks.size() == 0) {
      return;
    }
    curTask = std::move(tasks.top());
    tasks.pop();

    if (curTask.type == TaskType::CREATED) {
      void *top = curTask.stackTop;
      asm volatile("mov sp, %[rs] \n" : : [rs] "r"(top) : "memory");

      curTask.func();
      longjmp(buf, 1);
    } else {
      longjmp(*curTask.buf, 1);
    }
  }

  void workerThread() {
    setjmp(curTasks[std::this_thread::get_id()].buf)    ;
  }

public:
  static std::stack<Task<T>> tasks;
  static std::unordered_map<std::thread::id, Task<T>> curTasks;
  static std::unordered_map<std::thread::id, ;
  static std::mutex mut;
  static std::unordered_map<std::thread::id, jmp_buf> bufs;
  
  // All the threads in the thread pool
  std::vector<std::thread> threads;
  bool finish = false;

  SimpleContScheduler() {}

  T run(std::function<T()> func, int n) {
    tasks.emplace(std::move(Task(func)));
    for (int i = 0; i < n - 1; i++) {
      threads.emplace_back(&SimpleContScheduler::workerThread, this);
    }
    setjmp(buf);
    schedule();
  }

  void spawn(std::function<T()> func) {
    if (setjmp(*curTask.buf) == 0) {
      curTask.type = TaskType::CONTINUATION;
      tasks.emplace(std::move(Task(func)));
      tasks.emplace(std::move(curTask));
      longjmp(buf, 1);
    }
  }
};

template <typename T> std::stack<Task<T>> SimpleContScheduler<T>::tasks;

template <typename T> std::unordered_map<std::thread::id, Task<T>> SimpleContScheduler<T>::curTasks;

template <typename T> jmp_buf SimpleContScheduler<T>::buf;

template <typename T> std::mutex SimpleContScheduler<T>::mut;

template <typename T> std::unordered_map<std::thread::id, jmp_buf> SimpleContScheduler<T>::bufs;

#endif