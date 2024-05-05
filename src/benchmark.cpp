/**
 * @file benchmark.cppp
 * @author Yonah Goldberg (ygoldber@andrew.cmu.edu)
 * @author Jack Ellinger (jellinge@andrew.cmu.edu)
 * 
 * @brief this is the main file executed to benchmark all of our schedulers. We use
 * Google's benchmarking library. Information can be found here:
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
#include "tests/nqueens.hpp"
#include "tests/quicksort.hpp"
#include "tests/rectmul.hpp"
#include "tests/nbody.hpp"
#include "tests/heat.hpp"
#include "tests/pfor.hpp"

// Number of threads to spawn when running a test program
const int NUM_THREADS = 8;

// Utility function for quicksort to ensure an array is sorted
bool isSorted(const std::vector<int>& vec) {
  // Check if the vector has less than 2 elements, as a vector with 0 or 1 element is always sorted
  if (vec.size() < 2) {
    return true;
  }

  // Iterate through the vector and compare each element with the next one
  for (size_t i = 0; i < vec.size() - 1; ++i) {
    if (vec[i] > vec[i + 1]) {
      return false; // If any element is greater than its next element, the vector is not sorted
    }
  }

  return true; // If no out-of-order elements are found, the vector is sorted
}

// Custom assert function. We want to fail if any of our programs we test
// are not correct.
void assertTrue(bool condition, const std::string& message) {
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
static void initChildSchedulerLF(const benchmark::State &state) {
  scheduler = &childSchedulerLF;
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
    scheduler->run([&arr] { return quicksort(arr.data(), arr.data() + arr.size()); }, NUM_THREADS);
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
  char *copy = new char[n];
  std::copy(a, a + n, copy);
  for (auto _ : state) {
    scheduler->run([n, a] {return nqueens(n, 0, a);}, NUM_THREADS);
    a = copy;
  }
  delete[] a;
}

static void BM_Rectmul(benchmark::State &state) {
  int x = state.range(0);
  for (auto _ : state) {
    scheduler->run([x] { return rectmul(x,x,x); }, NUM_THREADS);
  }
}

static void BM_PFor(benchmark::State &state) {
  int x = state.range(0);
  for (auto _ : state) {
    scheduler->run([x] { return pfor(x); }, NUM_THREADS);
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

  std::vector<Particle> copy(particles);

  // Run the simulation for benchmarking
  for (auto _ : state) {
      // Run the n-body simulation
      scheduler->run([&particles] {
          simulateNBody(particles);
          return 0; 
      }, NUM_THREADS);
      state.PauseTiming();
      particles = copy;
      state.ResumeTiming();
  }
}

static void BM_Heat(benchmark::State &state) {
  int n = state.range(0);
  std::vector<Particle> particles;

  // Generate random input for the simulation
  // int nx = 4096;
  // int ny = 1024;
  // int nt = 100;
  // double xu = 0.0;
  // double xo = 1.570796326794896558;
  // double yu = 0.0;
  // double yo = 1.570796326794896558;
  // double tu = 0.0;
  // double to = 0.0000001;
  // int leafmaxcol = 1;
  int nx = 16384;
  int ny = 4096;
  int nt = 400;
  double xu = 0.0;
  double xo = 1.570796326794896558;
  double yu = 0.0;
  double yo = 1.570796326794896558;
  double tu = 0.0;
  double to = 0.0000001;
  int leafmaxcol = 1;

  // Run the simulation for benchmarking
  for (auto _ : state) {
    // Run the n-body simulation
    scheduler->run([=] {
      return heat(nx, ny, nt, xu, xo, yu, yo, tu, to, leafmaxcol);
    }, NUM_THREADS);
  }
}

// Configuration to benchmark quicksort on all schedulers

BENCHMARK(BM_Quicksort)
    ->Unit(benchmark::kMillisecond)
    ->Arg(5000000)
    ->Iterations(10)
    ->Setup(initNoSpawnScheduler)
    ->Name("NoSpawnScheduler Quicksort");
BENCHMARK(BM_Quicksort)
    ->Unit(benchmark::kMillisecond)
    ->Arg(5000000)
    ->Iterations(10)
    ->Setup(initSimpleScheduler)
    ->Name("SimpleScheduler Quicksort");
BENCHMARK(BM_Quicksort)
    ->Unit(benchmark::kMillisecond)
    ->Arg(5000000)
    ->Iterations(10)
    ->Setup(initChildScheduler)
    ->Name("ChildScheduler Quicksort");
BENCHMARK(BM_Quicksort)
    ->Unit(benchmark::kMillisecond)
    ->Arg(5000000)
    ->Iterations(10)
    ->Setup(initChildSchedulerLF)
    ->Name("ChildSchedulerLF Quicksort");

// Configuration to benchmark fib on all schedulers
BENCHMARK(BM_Fib)
    ->Unit(benchmark::kMillisecond)
    ->Arg(45)
    ->Iterations(5)
    ->Setup(initNoSpawnScheduler)
    ->Name("NoSpawnScheduler Fib");
BENCHMARK(BM_Fib)
    ->Unit(benchmark::kMillisecond)
    ->Arg(45)
    ->Iterations(5)
    ->Setup(initSimpleScheduler)
    ->Name("SimpleScheduler Fib");
BENCHMARK(BM_Fib)
    ->Unit(benchmark::kMillisecond)
    ->Arg(45)
    ->Iterations(5)
    ->Setup(initChildScheduler)
    ->Name("ChildScheduler Fib");
BENCHMARK(BM_Fib)
    ->Unit(benchmark::kMillisecond)
    ->Arg(45)
    ->Iterations(5)
    ->Setup(initChildSchedulerLF)
    ->Name("ChildSchedulerLF Fib");

BENCHMARK(BM_NQueens)
    ->Unit(benchmark::kMillisecond)
    ->Arg(14)
    ->Iterations(1)
    ->Setup(initNoSpawnScheduler)
    ->Name("NoSpawnScheduler N-Queens");
BENCHMARK(BM_NQueens)
    ->Unit(benchmark::kMillisecond)
    ->Arg(14)
    ->Iterations(1)
    ->Setup(initSimpleScheduler)
    ->Name("SimpleScheduler N-Queens");
BENCHMARK(BM_NQueens)
    ->Unit(benchmark::kMillisecond)
    ->Arg(14)
    ->Iterations(1)
    ->Setup(initChildScheduler)
    ->Name("ChildScheduler N-Queens");
BENCHMARK(BM_NQueens)
    ->Unit(benchmark::kMillisecond)
    ->Arg(14)
    ->Iterations(1)
    ->Setup(initChildSchedulerLF)
    ->Name("ChildSchedulerLF N-Queens");

BENCHMARK(BM_Rectmul)
    ->Unit(benchmark::kMillisecond)
    ->Arg(64)
    ->Iterations(1)
    ->Setup(initNoSpawnScheduler)
    ->Name("NoSpawnScheduler Rectmul");
BENCHMARK(BM_Rectmul)
    ->Unit(benchmark::kMillisecond)
    ->Arg(64)
    ->Iterations(1)
    ->Setup(initSimpleScheduler)
    ->Name("SimpleScheduler Rectmul");
BENCHMARK(BM_Rectmul)
    ->Unit(benchmark::kMillisecond)
    ->Arg(64)
    ->Iterations(1)
    ->Setup(initChildScheduler)
    ->Name("ChildScheduler Rectmul");
BENCHMARK(BM_Rectmul)
    ->Unit(benchmark::kMillisecond)
    ->Arg(64)
    ->Iterations(1)
    ->Setup(initChildSchedulerLF)
    ->Name("ChildSchedulerLF Rectmul");

BENCHMARK(BM_NBody)
    ->Unit(benchmark::kMillisecond)
    ->Arg(100000)
    ->Setup(initNoSpawnScheduler)
    ->Name("NoSpawnScheduler Nbody");
BENCHMARK(BM_NBody)
    ->Unit(benchmark::kMillisecond)
    ->Arg(100000)
    ->Setup(initSimpleScheduler)
    ->Name("SimpleScheduler Nbody");
BENCHMARK(BM_NBody)
    ->Unit(benchmark::kMillisecond)
    ->Arg(100000)
    ->Setup(initChildScheduler)
    ->Name("ChildScheduler Nbody");
BENCHMARK(BM_NBody)
    ->Unit(benchmark::kMillisecond)
    ->Arg(100000)
    ->Setup(initChildScheduler)
    ->Name("ChildSchedulerLF Nbody");

BENCHMARK(BM_Heat)
    ->Unit(benchmark::kMillisecond)
    ->Arg(1)
    ->Setup(initNoSpawnScheduler)
    ->Name("NoSpawnScheduler Heat");
BENCHMARK(BM_Heat)
    ->Unit(benchmark::kMillisecond)
    ->Arg(1)
    ->Setup(initSimpleScheduler)
    ->Name("SimpleScheduler Heat");
BENCHMARK(BM_Heat)
    ->Unit(benchmark::kMillisecond)
    ->Arg(1)
    ->Setup(initChildScheduler)
    ->Name("ChildScheduler Heat");
BENCHMARK(BM_Heat)
    ->Unit(benchmark::kMillisecond)
    ->Arg(1)
    ->Setup(initChildScheduler)
    ->Name("ChildSchedulerLF Heat");

BENCHMARK(BM_PFor)
    ->Unit(benchmark::kMillisecond)
    ->Arg(500)
    ->Iterations(10)
    ->Setup(initNoSpawnScheduler)
    ->Name("NoSpawnScheduler PFor");
BENCHMARK(BM_PFor)
    ->Unit(benchmark::kMillisecond)
    ->Arg(500)
    ->Iterations(10)
    ->Setup(initSimpleScheduler)
    ->Name("SimpleScheduler PFor");
BENCHMARK(BM_PFor)
    ->Unit(benchmark::kMillisecond)
    ->Arg(500)
    ->Iterations(10)
    ->Setup(initChildScheduler)
    ->Name("ChildScheduler PFor");
BENCHMARK(BM_PFor)
    ->Unit(benchmark::kMillisecond)
    ->Arg(500)
    ->Iterations(10)
    ->Setup(initChildScheduler)
    ->Name("ChildSchedulerLF PFor");

BENCHMARK_MAIN();