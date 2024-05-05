#include <algorithm>
#include <chrono>
#include <cilk/cilk.h>
#include <iostream>

// Custom assert function. We want to fail if any of our programs we test
// are not correct.
void assertTrue(bool condition, const std::string &message) {
  if (!condition) {
    std::cerr << "Assertion failed: " << message << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

long fibSeq(long n) {
  if (n < 2) {
    return n;
  } else {
    return fibSeq(n - 1) + fibSeq(n - 2);
  }
}

long fib(long n) {
  if (n < 20) {
    return fibSeq(n);
  } else {
    long fib1 = cilk_spawn fib(n - 1);
    long fib2 = fib(n - 2);
    cilk_sync;
    return fib1 + fib2;
  }
}

int main() {
  auto start = std::chrono::steady_clock::now();
  long res = fib(45);
  auto end = std::chrono::steady_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << res << std::endl;
  std::cout << "Fib time: " << duration.count() << " ms" << std::endl;
}