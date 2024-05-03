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

  void push(Task<T>job) {
    int bottom = bottomIndex.load(std::memory_order_seq_cst);
    queue[bottom] = std::move(job);
    bottomIndex.store(bottom + 1, std::memory_order_seq_cst);
  }

  std::optional<Task<T>> pop() {
    int bottom = bottomIndex.load(std::memory_order_seq_cst);
    int top = topIndex.load(std::memory_order_seq_cst);
    bottom = std::max(0, bottom - 1);
    bottomIndex.store(bottom, std::memory_order_seq_cst);
    if (top <= bottom) {
      Task<T>& job = queue[bottom];
      if (top != bottom) {
        Task<T> task = std::move(job);
        return task;
      }
      if (topIndex.compare_exchange_strong(top, top + 1,
                                           std::memory_order_seq_cst)) {
        bottomIndex.store(top + 1, std::memory_order_release);
        Task<T> task = std::move(job);
        return task;
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
      Task<T>& job = queue[top];
      if (topIndex.compare_exchange_strong(top, top + 1,
                                           std::memory_order_seq_cst)) {
        Task<T> task = std::move(job);
        return task;
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