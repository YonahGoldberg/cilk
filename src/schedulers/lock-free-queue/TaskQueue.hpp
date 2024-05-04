#pragma once

#include "Task.hpp"
#include <algorithm>
#include <atomic>
#include <vector>
#include <optional>

template <typename T> class TaskQueue {
public:
  TaskQueue() {
    bottomIndex = 0;
    topIndex = 0;
    queue.resize(100000);
  }

  TaskQueue(const TaskQueue&) = delete; // Disable copy constructor
  TaskQueue(TaskQueue&& other) noexcept
      : bottomIndex(other.bottomIndex.load(std::memory_order_relaxed)),
        topIndex(other.topIndex.load(std::memory_order_relaxed)),
        queue(std::move(other.queue)) {} // Allow move constructor
  TaskQueue& operator=(const TaskQueue&) = delete; // Disable copy assignment
  TaskQueue& operator=(TaskQueue&&) = delete; // Disable move assignment

  void push(Task<T> job) {
    int bottom = bottomIndex.load(std::memory_order_seq_cst);
    queue[bottom] = std::move(job);
    bottomIndex.store(bottom + 1, std::memory_order_seq_cst);
  }

  std::optional<Task<T>> pop() {
    std::cout << "in pop\n";
    int bottom = bottomIndex.load(std::memory_order_seq_cst);
    int top = topIndex.load(std::memory_order_seq_cst);
    std::cout << bottom << std::endl;
    std::cout << top << std::endl;
    bottom = std::max(0, bottom - 1);
    bottomIndex.store(bottom, std::memory_order_seq_cst);
    if (top <= bottom) {
      Task<T>* job = &queue[bottom];
      if (top != bottom) {
        return std::move(*job);
      }
      if (topIndex.compare_exchange_strong(top, top + 1,
                                           std::memory_order_seq_cst)) {
        bottomIndex.store(top + 1, std::memory_order_release);
        std::cout << "got task\n";
        return std::move(*job);
      }
      bottomIndex.store(top + 1, std::memory_order_release);
      return std::nullopt;
    } else {
      bottomIndex.store(top, std::memory_order_seq_cst);
      return std::nullopt;
    }
  }

  std::optional<Task<T>> steal() {
    int top = topIndex.load(std::memory_order_seq_cst);
    int bottom = bottomIndex.load(std::memory_order_seq_cst);
    if (top < bottom) {
      Task<T>* job = &queue[top];
      if (topIndex.compare_exchange_strong(top, top + 1,
                                           std::memory_order_seq_cst)) {
        return std::move(*job);
      }
      return std::nullopt;
    } else {
      return std::nullopt;
    }
  }

  size_t size() const { return static_cast<size_t>(bottomIndex - topIndex); }

  void clear() {
    bottomIndex = 0;
    topIndex = 0;
  }

private:
  int maxJobs;
  std::atomic_int bottomIndex;
  std::atomic_int topIndex;
  std::vector<Task<T>> queue;
};