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

SimpleScheduler<int> scheduler;
// SimpleCSScheduler<int> scheduler;

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

    scheduler.steal(std::move(x));
    scheduler.steal(std::move(y));
  }

  return 0;
}

// using Matrix = std::vector<std::vector<double>>;

// void multiplyCell(const Matrix& A, const Matrix& B, Matrix& C, size_t row,
// size_t col) {
//     double sum = 0.0;
//     for (size_t k = 0; k < A.size(); k++) {
//         sum += A[row][k] * B[k][col];
//     }
//     C[row][col] = sum;
// }

// void matrixMultiplyParallel(const Matrix& A, const Matrix& B, Matrix& C) {
//     for (size_t i = 0; i < A.size(); i++) {
//         for (size_t j = 0; j < B.size(); j++) {
//             auto result = scheduler.spawn([A, B, C, i, j] { return
//             multiplyCell(A, B, C, i, j); })
//         }
//     }
//     // cilk_sync;
// }

static void init8Threads(const benchmark::State &state) { scheduler.init(8); }

static void joinThreads(const benchmark::State &state) {
  scheduler.~SimpleScheduler();
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
    ->Setup(init8Threads);

BENCHMARK_MAIN();