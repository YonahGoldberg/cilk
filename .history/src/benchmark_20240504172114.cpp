/**
 * @file benchmark.cppp
 * @author Yonah Goldberg (ygoldber@andrew.cmu.edu)
 * @author Jack Ellinger (jellinge@andrew.cmu.edu)
 *
 * @brief this is the main file executed to benchmark all of our schedulers. We
 * use Google's benchmarking library. Information can be found here:
 * https://github.com/google/benchmark/blob/main/docs/user_guide.md
 *
 */

#include <algorithm>
#include <benchmark/benchmark.h>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

#include "scheduler_instance.hpp"
#include "tests/fib.hpp"
#include "tests/nbody.hpp"
#include "tests/nqueens.hpp"
#include "tests/quicksort.hpp"
#include "tests/rectmul.hpp"

// Number of threads to spawn when running a test program
const int NUM_THREADS = 8;

// Utility function for quicksort to ensure an array is sorted
bool isSorted(const std::vector<int> &vec) {
  // Check if the vector has less than 2 elements, as a vector with 0 or 1
  // element is always sorted
  if (vec.size() < 2) {
    return true;
  }

  // Iterate through the vector and compare each element with the next one
  for (size_t i = 0; i < vec.size() - 1; ++i) {
    if (vec[i] > vec[i + 1]) {
      return false; // If any element is greater than its next element, the
                    // vector is not sorted
    }
  }

  return true; // If no out-of-order elements are found, the vector is sorted
}

// Custom assert function. We want to fail if any of our programs we test
// are not correct.
void assertTrue(bool condition, const std::string &message) {
  if (!condition) {
    std::cerr << "Assertion failed: " << message << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

// Initialization functions that run at the beginning of each test.
// We set the global scheduler used in all tests to a specific scheduler
// we want to test.
static void initSimpleScheduler(const benchmark::State &state) {
  scheduler = &simpleScheduler;
}
static void initChildScheduler(const benchmark::State &state) {
  scheduler = &childScheduler;
}
static void initNoSpawnScheduler(const benchmark::State &state) {
  scheduler = &noSpawnScheduler;
}

// Benchmark quicksort. Generate a random vector of integers and sort it!
static void BM_Quicksort(benchmark::State &state) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(1, 1000); // Adjust range as needed

  std::vector<int> arr(state.range(0));
  for (auto &elem : arr) {
    elem = dist(gen); // Fill the array with random integers
  }
  std::vector<int> copy(arr);

  for (auto _ : state) {
    scheduler->run(
        [&arr] { return quicksort(arr.data(), arr.data() + arr.size()); },
        NUM_THREADS);
    state.PauseTiming();
    assertTrue(isSorted(arr), "Quicksort");
    arr = copy;
    state.ResumeTiming();
  }
}

// Benchmark fibonacci. We test the inefficient O(2^n) recursive
// algorithm to find the nth fibonacci number.
static void BM_Fib(benchmark::State &state) {
  for (auto _ : state) {
    int x = state.range(0);
    int res = scheduler->run([x] { return fib(x); }, NUM_THREADS);
    state.PauseTiming();
    assertTrue(res == fibSeq(x), "Fib");
    state.ResumeTiming();
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

double getRandomDouble(double min, double max) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dist(min, max);
  return dist(gen);
}

static void BM_NBody(benchmark::State &state) {
  int n = state.range(0);
  std::vector<Particle> particles;

  // Generate random input for the simulation
  for (int i = 0; i < n; ++i) {
    double x = getRandomDouble(-100.0, 100.0);
    double y = getRandomDouble(-100.0, 100.0);
    double vx = getRandomDouble(-10.0, 10.0);
    double vy = getRandomDouble(-10.0, 10.0);
    double m = getRandomDouble(1, 1000.0);
    particles.push_back(Particle(x, y, vx, vy, m));
  }

  // Run the simulation for benchmarking
  for (auto _ : state) {
    // Run the n-body simulation
    scheduler->run(
        [&particles] {
          simulateNBody(particles);
          return 0;
        },
        NUM_THREADS);

    // scheduler->run([x] { return fib(x); }, NUM_THREADS);
  }
}

// Configuration to benchmark quicksort on all schedulers
BENCHMARK(BM_NBody)
    ->Unit(benchmark::kMillisecond)
    ->Arg(1000)
    ->Setup(initNoSpawnScheduler)
    ->Name("NoSpawnScheduler Nbody");

BENCHMARK(BM_Quicksort)
    ->Unit(benchmark::kMillisecond)
    ->Arg(1000000)
    ->Setup(initNoSpawnScheduler)
    ->Name("NoSpawnScheduler Quicksort");
BENCHMARK(BM_Quicksort)
    ->Unit(benchmark::kMillisecond)
    ->Arg(1000000)
    ->Setup(initSimpleScheduler)
    ->Name("SimpleScheduler Quicksort");
BENCHMARK(BM_Quicksort)
    ->Unit(benchmark::kMillisecond)
    ->Arg(1000000)
    ->Setup(initChildScheduler)
    ->Name("ChildScheduler Quicksort");

// Configuration to benchmark fib on all schedulers
BENCHMARK(BM_Fib)
    ->Unit(benchmark::kMillisecond)
    ->Arg(30)
    ->Setup(initNoSpawnScheduler)
    ->Name("NoSpawnScheduler Fib");
BENCHMARK(BM_Fib)
    ->Unit(benchmark::kMillisecond)
    ->Arg(30)
    ->Setup(initSimpleScheduler)
    ->Name("SimpleScheduler Fib");
BENCHMARK(BM_Fib)
    ->Unit(benchmark::kMillisecond)
    ->Arg(30)
    ->Setup(initChildScheduler)
    ->Name("ChildScheduler Fib");

// BENCHMARK(BM_NQueens)
//     ->Unit(benchmark::kMillisecond)
//     ->Arg(10)
//     ->Setup(initNoSpawnScheduler)
//     ->Name("NoSpawnScheduler N-Queens");
// BENCHMARK(BM_NQueens)
//     ->Unit(benchmark::kMillisecond)
//     ->Arg(10)
//     ->Setup(initSimpleScheduler)
//     ->Name("SimpleScheduler N-Queens");
// BENCHMARK(BM_NQueens)
//     ->Unit(benchmark::kMillisecond)
//     ->Arg(10)
//     ->Setup(initChildScheduler)
//     ->Name("ChildScheduler N-Queens");

// BENCHMARK(BM_Rectmul)
//     ->Unit(benchmark::kMillisecond)
//     ->Arg(64)
//     ->Setup(initNoSpawnScheduler)
//     ->Name("NoSpawnScheduler Rectmul");
// BENCHMARK(BM_Rectmul)
//     ->Unit(benchmark::kMillisecond)
//     ->Arg(64)
//     ->Setup(initSimpleScheduler)
//     ->Name("SimpleScheduler Rectmul");
// BENCHMARK(BM_Rectmul)
//     ->Unit(benchmark::kMillisecond)
//     ->Arg(64)
//     ->Setup(initChildScheduler)
//     ->Name("ChildScheduler Rectmul");

BENCHMARK_MAIN();