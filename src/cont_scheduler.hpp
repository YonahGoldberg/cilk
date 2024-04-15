#ifndef SIMPLE_CONT_SCHEDULER_HPP
#define SIMPLE_CONT_SCHEDULER_HPP

#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <csetjmp>
#include <stack>

const size_t STACK_SIZE = 16 * 1024;

enum TaskType {
  CREATED,
  CONTINUATION,
};

template<typename T>
struct Task {
    std::function<T()> func;
    void* stackBottom;
    void* stackTop;
    TaskType type;
    jmp_buf buf;
};

template<typename T>
Task<T>* createTask(std::function<T()> func) {
    Task<T>* task = new Task<T>();
    task->func = func;
    task->stackBottom = new char[STACK_SIZE];
    task->stackTop = static_cast<char*>(task->stackBottom) + STACK_SIZE;
    task->type = TaskType::CREATED;
    return task;
}

template <typename T>
class SimpleContScheduler {
private:
  void schedule() {
    if (tasks.size() == 0) {
      return;
    }
    curTask = tasks.top();
    tasks.pop();

    if (curTask->type == TaskType::CREATED) {
      void* top = curTask->stackTop;
      asm volatile(
        "mov sp, %[rs] \n"
        :
        : [rs] "r" (top)
        : "memory"
      );

      curTask->func();
      longjmp(buf, 1);
    } else {
      longjmp(curTask->buf, 1);
    }
  }

public:
  static std::stack<Task<T>*> tasks;
  static Task<T>* curTask;
  static jmp_buf buf;

  SimpleContScheduler() {}

  T run(std::function<T()> func) {
    tasks.push(createTask(func));
    setjmp(buf);
    schedule();
  }

  void spawn(std::function<T()> func) {
    if (setjmp(curTask->buf) == 0) {
      curTask->type = TaskType::CONTINUATION;
      tasks.push(createTask(func));
      tasks.push(curTask);
      longjmp(buf, 1);
    }
  }
};

template<typename T>
std::stack<Task<T>*> SimpleContScheduler<T>::tasks;

template<typename T>
Task<T>* SimpleContScheduler<T>::curTask;

template<typename T>
jmp_buf SimpleContScheduler<T>::buf;

#endif