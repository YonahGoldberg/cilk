#include <algorithm>
#include <benchmark/benchmark.h>
#include <cassert>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

#include "scheduler.hpp"
#include "simple_cs_scheduler.hpp"
#include "simple_scheduler.hpp"

// SimpleScheduler<int> scheduler;
SimpleCSScheduler<int> scheduler;

int fib(int n) {
  if (n < 2) {
    return n;
  } else {
    auto x = scheduler.spawn([n] { return fib(n - 1); });
    int y = fib(n - 2);
    return (x.get() + y);
  }
}

int quicksort(int *begin, int *end) {
  if (begin != end) {
    end--;
    int pivot = *end;
    auto middle =
        std::partition(begin, end, [pivot](int x) { return x < pivot; });
    std::swap(*end, *middle);

    auto x =
        scheduler.spawn([begin, middle]() { return quicksort(begin, middle); });
    auto y = scheduler.spawn(
        [begin, middle, end]() mutable { return quicksort(++middle, ++end); });

    std::cout << "waiting on get" << std::endl;
    scheduler.steal(std::move(x));
    scheduler.steal(std::move(y));
    std::cout << "got get" << std::endl;
  }

  return 0;
}

static void init8Threads(const benchmark::State &state) { scheduler.init(8); }

static void joinThreads(const benchmark::State &state) {
  scheduler.~SimpleCSScheduler();
}

static void BM_Quicksort(benchmark::State &state) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(1, 1000); // Adjust range as needed

  std::vector<int> arr(state.range(0));
  for (auto &elem : arr) {
    elem = dist(gen); // Fill the array with random integers
  }

  for (auto _ : state) {
    quicksort(arr.data(), arr.data() + arr.size());
  }
}

BENCHMARK(BM_Quicksort)
    ->Unit(benchmark::kMillisecond)
    ->Range(2 << 5, 2 << 18)
    ->Setup(init8Threads)
    ->Teardown(joinThreads);

BENCHMARK_MAIN();