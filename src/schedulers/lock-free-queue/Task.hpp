// Task.h
#pragma once

#include <functional>
#include <future>

template <typename T> struct Task {
  std::packaged_task<T()> func;
};