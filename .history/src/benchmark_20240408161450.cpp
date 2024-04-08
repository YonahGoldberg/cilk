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

// SimpleScheduler<int> scheduler;

// int quicksort(int *begin, int *end) {
//   if (begin != end) {
//     end--;
//     int pivot = *end;
//     auto middle =
//         std::partition(begin, end, [pivot](int x) { return x < pivot; });
//     std::swap(*end, *middle);

//     auto x = scheduler.spawn(
//         [begin, middle]() { return quicksort(begin, middle); });
//     auto y = scheduler.spawn(
//         [begin, middle, end]() mutable { return quicksort(++middle, ++end); });

//     scheduler.steal(std::move(x));
//     scheduler.steal(std::move(y));
//   }

//   return 0;
// }

// using Matrix = std::vector<std::vector<double>>;

// void multiplyCell(const Matrix& A, const Matrix& B, Matrix& C, size_t row, size_t col) {
//     double sum = 0.0;
//     for (size_t k = 0; k < A.size(); k++) {
//         sum += A[row][k] * B[k][col];
//     }
//     C[row][col] = sum;
// }

// void matrixMultiplyParallel(const Matrix& A, const Matrix& B, Matrix& C) {
//     for (size_t i = 0; i < A.size(); i++) {
//         for (size_t j = 0; j < B.size(); j++) {
//             auto result = scheduler.spawn([A, B, C, i, j] { return multiplyCell(A, B, C, i, j); }) 
//         }
//     }
//     scheduler.sync();
// }

// // Simple n-body problem
// struct Particle {
//     double x, y, z; 
//     double vx, vy, vz; 
//     double mass;
// };

// void calculateForce(const Particle& a, const Particle& b, double& fx, double& fy, double& fz) {
//     double G = 9.8; 
//     double dx = b.x - a.x;
//     double dy = b.y - a.y;
//     double dz = b.z - a.z;
//     double distSq = dx*dx + dy*dy + dz*dz + 1e-10; // Add small value to prevent division by zero
//     double distSixth = distSq * distSq * distSq;
//     double force = G * a.mass * b.mass / distSixth;
//     fx += force * dx;
//     fy += force * dy;
//     fz += force * dz;
// }

// void updateParticle(Particle& a, const double fx, const double fy, const double fz, double deltaTime) {
//     // Update velocities based on force
//     a.vx += fx / a.mass * deltaTime;
//     a.vy += fy / a.mass * deltaTime;
//     a.vz += fz / a.mass * deltaTime;

//     // Update positions based on velocity
//     a.x += a.vx * deltaTime;
//     a.y += a.vy * deltaTime;
//     a.z += a.vz * deltaTime;
// }


// void nBodySimulationParallel(std::vector<Particle>& particles, double deltaTime) {
//     const size_t numParticles = particles.size();
//     std::vector<std::tuple<double, double, double>> forces(numParticles);

//     for (size_t i = 0; i < numParticles; i++) {
//         scheduler.spawn([&, i] {
//             double fx = 0.0, fy = 0.0, fz = 0.0;
//             for (size_t j = 0; j < numParticles; j++) {
//                 if (i != j) {
//                     calculateForce(particles[i], particles[j], fx, fy, fz);
//                 }
//             }
//             forces[i] = std::make_tuple(fx, fy, fz);
//             return 0;
//         });
//     }
//     scheduler.sync();

//     // Update particles in parallel
//     for (size_t i = 0; i < numParticles; i++) {
//         scheduler.spawn([&, i] {
//             auto [fx, fy, fz] = forces[i];
//             updateParticle(particles[i], fx, fy, fz, deltaTime);
//             return 0; // Placeholder return
//         });
//     }
//     scheduler.sync();
// }


// int ok(int n, char *a) {
//     for (int i = 0; i < n; i++) {
//         char p = a[i];
//         for (int j = i + 1; j < n; j++) {
//             char q = a[j];
//             if (q == p || q == p - (j - i) || q == p + (j - i))
//                 return 0;
//         }
//     }
//     return 1;
// }

// int nqueens(int n, int j, char *a) {
//     if (n == j) {
//         return 1;
//     }

//     std::vector<std::future<int>> futures;
//     int solNum = 0;

//     for (int i = 0; i < n; i++) {
//         auto b = std::make_unique<char[]>(j + 1);
//         memcpy(b.get(), a, j * sizeof(char));
//         b[j] = i;

//         if (ok(j + 1, b.get())) {
//             // Spawn a new task for exploring this partial solution
//             auto fut = scheduler.spawn([n, j, b = std::move(b)]() mutable {
//                 return nqueens(n, j + 1, b.get());
//             });
//             futures.push_back(std::move(fut));
//         }
//     }

//     // Wait for all spawned tasks to complete
//     scheduler.sync(); // Assuming your scheduler has a way to implicitly wait on all tasks or futures

//     // Collect results from all futures
//     for (auto &fut : futures) {
//         solNum += fut.get();
//     }

//     return solNum;
// }



static void init8Threads(const benchmark::State& state) {
  scheduler.init(8);
}

static void joinThreads(const benchmark::State& state) {
  scheduler.~SimpleScheduler();
}


static void BM_Quicksort(benchmark::State& state) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 1000); // Adjust range as needed

    std::vector<int> arr(state.range(0));
    for (auto& elem : arr) {
        elem = dist(gen); // Fill the array with random integers
    }

    for (auto _ : state) {
        quicksort(arr.data(), arr.data() + arr.size());
    }
}

BENCHMARK(BM_Quicksort)->Unit(benchmark::kMillisecond)->Range(2 << 5, 2 << 18)->Setup(init8Threads);

BENCHMARK_MAIN();