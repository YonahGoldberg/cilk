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
    jmp_buf* buf = (jmp_buf*)malloc(sizeof(jmp_buf));

    Task() : func([] {}) {}

    Task(std::function<T()> func)
        : func(std::move(func)), stackBottom(new char[STACK_SIZE]), stackTop(static_cast<char*>(stackBottom) + STACK_SIZE), type(TaskType::CREATED) {}
    
    Task(Task&& rhs) {
      func = rhs.func;
      stackBottom = std::exchange(rhs.stackBottom, nullptr);
      stackTop = rhs.stackTop;
      type = rhs.type;
      buf = rhs.buf;
    }

    Task& operator=(Task&& rhs) {
      Task copy(std::move(rhs));
      copy.swap(*this);
      return *this;
    }

    ~Task() {
        if (stackBottom != nullptr) {
            delete[] static_cast<char*>(stackBottom);
        }
    }

    void swap(Task& rhs) {
      std::swap(func, rhs.func);
      std::swap(stackBottom, rhs.stackBottom);
      std::swap(stackTop, rhs.stackTop);
      std::swap(type, rhs.type);
      std::swap(buf, rhs.buf);
    }
};

template <typename T>
class SimpleContScheduler {
private:
  std::queue<Task<void>> tasks;
  Task<T> curTask;
  jmp_buf buf;

  void schedule() {
    if (tasks.size() == 0) return;
    std::cout << "here1\n";
    curTask = std::move(tasks.front());
    std::cout << "here2\n";
    tasks.pop();
    std::cout << "here3\n";

    if (curTask.type == TaskType::CREATED) {
      std::cout << "here4\n";
      void* top = curTask.stackTop;
      std::cout << top << std::endl;

      asm volatile(
        "mov sp, %[rs] \n"
        :
        : [rs] "r" (top)
        : "memory"
      );
      std::cout << "here4.5\n";

      curTask.func();
      std::cout << "here5\n";
      longjmp(buf, 1);
    } else {
      longjmp(*curTask.buf, 1);
    }
  }

public:
  SimpleContScheduler() {}

  T run(std::function<T()> func) {
    tasks.emplace(std::move(Task(func)));
    if (setjmp(buf) == 0) {
      std::cout << "scheduling" << std::endl;
      schedule();
    }
  }

  std::future<T> spawn(std::function<T()> func) {
    if (setjmp(*curTask.buf) == 0) {
      curTask.type = TaskType::CONTINUATION;
      tasks.emplace(std::move(curTask));
      tasks.emplace(std::move(Task(func)));
      longjmp(buf, 1);
    }
  }
};

#endif