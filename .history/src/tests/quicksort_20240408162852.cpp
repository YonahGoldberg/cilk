#include "quicksort.hpp"
#include "scheduler_instance.hpp"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <cassert>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

int quicksort(int *begin, int *end) {
  if (begin != end) {
    end--;
    int pivot = *end;
    auto middle =
        std::partition(begin, end, [pivot](int x) { return x < pivot; });
    std::swap(*end, *middle);

    auto x = scheduler->spawn(
        [begin, middle]() { return quicksort(begin, middle); });
    auto y = scheduler->spawn(
        [begin, middle, end]() mutable { return quicksort(++middle, ++end); });

    scheduler->steal(std::move(x));
    scheduler->steal(std::move(y));
  }

  return 0;
}