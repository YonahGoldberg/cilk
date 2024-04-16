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

// static void BM_Nqueens(benchmark::State &state) {
//     // Test cases for N-Queens
//     const std::vector<std::vector<int>> test_cases = {
//         {4, 0, 1, 2, 3},   // N = 4, unique solution
//         {8, 0, 4, 7, 5, 2, 6, 1, 3},   // N = 8, unique solution
//         {1, 0},   // N = 1, trivial case
//         {2, -1},  // N = 2, no solution
//         {6, -1}   // N = 6, no solution
//     };

//     for (auto _ : state) {
//         for (const auto &test_case : test_cases) {
//             int n = test_case[0]; // Size of the chessboard
//             std::vector<char> a(test_case.begin() + 1, test_case.end()); // Positions of the queens

//             // Call the nqueens function with the generated input
//             nqueens(n, 0, a.data());
//         }
//     }
// }
static void BM_Nqueens(benchmark::State &state) {
    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_int_distribution<int> dist(4, 8); // Adjust the range as needed

    const int n = state.range(0); // Randomly choose the size of the chessboard

    char *a = new char[n]; // Dynamic allocation for the chessboard
    for (auto _ : state) {
        // Fill the chessboard with random positions for the queens
        // for (int i = 0; i < n; ++i) {
        //     a[i] = dist(gen); // Randomly choose the position of the queens
        // }

        // Call the nqueens function with the generated input
        nqueens(n, 0, a);

        // Cleanup allocated memory
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
    ->Name("SimpleScheduler Nqueens");
BENCHMARK(BM_Nqueens)
    ->Unit(benchmark::kMillisecond)
    ->Range(10, 15)
    ->Setup(initSimpleCSScheduler)
    ->Teardown(cleanupSimpleCSScheduler)
    ->Name("SimpleCSScheduler Nqueens");

BENCHMARK_MAIN();