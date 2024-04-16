#include <algorithm>
#include <benchmark/benchmark.h>
#include <cassert>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

#include "scheduler_instance.hpp"
#include "tests/fib.hpp"
#include "tests/nqueens.hpp"
#include "tests/quicksort.hpp"
#include "tests/rectmul.hpp"

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
static void BM_NQueens(benchmark::State &state) {
  int n = state.range(0);
  char *a = new char[n];
  for (auto _ : state) {
    nqueens(n, 0, a);
  }
  delete[] a;
}
static void BM_Rectmul(benchmark::State &state) {
  int x = state.range(0);
  for (auto _ : state) {
    rectmul(x, x, x);
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
BENCHMARK(BM_NQueens)
    ->Unit(benchmark::kMillisecond)
    ->Arg(5)
    ->Arg(8)
    ->Arg(10)
    ->Setup(initSimpleScheduler)
    ->Teardown(cleanupSimpleScheduler)
    ->Name("SimpleScheduler N-Queens");
BENCHMARK(BM_NQueens)
    ->Unit(benchmark::kMillisecond)
    ->Arg(5)
    ->Arg(8)
    ->Arg(10)
    ->Setup(initSimpleCSScheduler)
    ->Teardown(cleanupSimpleCSScheduler)
    ->Name("SimpleCSScheduler N-Queens");
// BENCHMARK(BM_Rectmul)
//     ->Unit(benchmark::kMillisecond)
//     ->Arg(64)
//     ->Setup(initSimpleScheduler)
//     ->Teardown(cleanupSimpleScheduler)
//     ->Name("SimpleScheduler Rectmul");
// BENCHMARK(BM_Rectmul)
//     ->Unit(benchmark::kMillisecond)
//     ->Arg(64)
//     ->Setup(initSimpleCSScheduler)
//     ->Teardown(cleanupSimpleCSScheduler)
//     ->Name("SimpleCSScheduler Rectmul");

BENCHMARK_MAIN();