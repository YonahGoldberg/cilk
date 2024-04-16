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

#include "tests/fib.hpp"
#include "tests/quicksort.hpp"
#include "tests/nqueens.hpp"

SimpleScheduler<int> simpleScheduler;
SimpleCSScheduler<int> simpleCSScheduler;
Scheduler<int> *scheduler = &simpleScheduler;



static void initSimpleScheduler(const benchmark::State &state) {
  scheduler = &simpleScheduler;
  scheduler->init(8);
}
static void cleanupSimpleScheduler(const benchmark::State &state) {
  scheduler = &simpleScheduler;
  scheduler->cleanup();
}
static void initSimpleCSScheduler(const benchmark::State &state) {
  scheduler = &simpleCSScheduler;
  scheduler->init(8);
}
static void cleanupSimpleCSScheduler(const benchmark::State &state) {
  scheduler = &simpleCSScheduler;
  scheduler->cleanup();
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
static void BM_Fib(benchmark::State &state) {
  for (auto _ : state) {
    fib(state.range(0));
  }
}
static void BM_Nqueens(benchmark::State &state) {
  for (auto _ : state) {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    const int n = state.range(0);  // Get the size of the chessboard from state
    std::vector<char> board(n);

    // Fill the board with random queen positions
    for (int i = 0; i < n; ++i) {
        std::uniform_int_distribution<> dis(0, n-1);
        board[i] = dis(gen);
    }

    for (auto _ : state) {
        nqueens(n, 0, board.data());
    }
  }
}

BENCHMARK(BM_Quicksort)
    ->Unit(benchmark::kMillisecond)
    ->Range(2 << 5, 2 << 10)
    ->Setup(initSimpleScheduler)
    ->Teardown(cleanupSimpleScheduler)
    ->Name("SimpleScheduler Quicksort");
BENCHMARK(BM_Quicksort)
    ->Unit(benchmark::kMillisecond)
    ->Range(2 << 5, 2 << 10)
    ->Setup(initSimpleCSScheduler)
    ->Teardown(cleanupSimpleCSScheduler)
    ->Name("SimpleCSScheduler Quicksort");
BENCHMARK(BM_Fib)
    ->Unit(benchmark::kMillisecond)
    ->Range(10, 15)
    ->Setup(initSimpleScheduler)
    ->Teardown(cleanupSimpleScheduler)
    ->Name("SimpleScheduler Fib");
BENCHMARK(BM_Fib)
    ->Unit(benchmark::kMillisecond)
    ->Range(10, 15)
    ->Setup(initSimpleCSScheduler)
    ->Teardown(cleanupSimpleCSScheduler)
    ->Name("SimpleCSScheduler Fib");

BENCHMARK(BM_Nqueens)
    ->Unit(benchmark::kMillisecond)
    ->Range(10, 15)
    ->Setup(initSimpleScheduler)
    ->Teardown(cleanupSimpleScheduler)
    ->Name("SimpleScheduler Fib");
BENCHMARK(BM_Nqueens)
    ->Unit(benchmark::kMillisecond)
    ->Range(10, 15)
    ->Setup(initSimpleCSScheduler)
    ->Teardown(cleanupSimpleCSScheduler)
    ->Name("SimpleCSScheduler Fib");

BENCHMARK_MAIN();