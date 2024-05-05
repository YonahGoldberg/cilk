#pragma once

#include "Task.hpp"
#include <algorithm>
#include <atomic>
#include <vector>
#include <optional>

template <typename T> class TaskQueue {
public:
  TaskQueue() {
    backIndex = 0; // nothing in queue so initialze to 0
    frontIndex = 0; // nothing in queue so initialze to 0
    queue.resize(100000);
  }

  TaskQueue(const TaskQueue&) = delete; // Disable copy constructor
  TaskQueue(TaskQueue&& other) noexcept
      : backIndex(other.backIndex.load(std::memory_order_relaxed)),
        frontIndex(other.frontIndex.load(std::memory_order_relaxed)),
        queue(std::move(other.queue)) {} // Allow move constructor
  TaskQueue& operator=(const TaskQueue&) = delete; // Disable copy assignment
  TaskQueue& operator=(TaskQueue&&) = delete; // Disable move assignment

  // This thread tries to pop from back of the queue and adds to back of the queue allowing for LF
  std::optional<Task<T>> pop() {
    int back = backIndex.load(std::memory_order_seq_cst);
    if (back == 0) return std::nullopt;
    int front = frontIndex.load(std::memory_order_seq_cst);
    back = std::max(0, back - 1); // Error where it goes negative
    backIndex.store(back, std::memory_order_seq_cst); // reupdate it
    if (front <= back) {
      Task<T>* task = &queue[back];
      if (front != back) {
        return std::move(*task);
      } 
      // Try and update it, if not, report nullopt as it failed
      if (frontIndex.compare_exchange_strong(front, front + 1,
                                           std::memory_order_seq_cst)) {
        backIndex.store(front + 1, std::memory_order_release);
        return std::move(*task);
      }
      backIndex.store(front + 1, std::memory_order_release);
      return std::nullopt;
    } else {
      backIndex.store(front, std::memory_order_seq_cst);
      return std::nullopt;
    }
  }
  // This thread adds to back of queue to reduce contention and they steal from front
  void push(Task<T> task) {
    int back = backIndex.load(std::memory_order_seq_cst);
    queue[back] = std::move(task);
    backIndex.store(back + 1, std::memory_order_seq_cst);
  }

  // Steal from front of queue and try and steal it by using cmp_exchange
  std::optional<Task<T>> steal() {
    int front = frontIndex.load(std::memory_order_seq_cst);
    int back = backIndex.load(std::memory_order_seq_cst);
    if (front < back) {
      Task<T>* task = &queue[front];
      if (frontIndex.compare_exchange_strong(front, front + 1,
                                           std::memory_order_seq_cst)) {
        return std::move(*task);
      }
      // Did not successfully update the front index
      return std::nullopt;
    } else {
      return std::nullopt;
    }
  }


private:
  std::vector<Task<T>> queue; // the actual queue storing everything
  std::atomic_int backIndex; // Pointer for back index of the queue
  std::atomic_int frontIndex; // Pointer for the front index of the queue
  
};