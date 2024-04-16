#include "fib.hpp"
#include "../scheduler_instance.hpp"

int fib(int n) {
  if (n < 2) {
    return n;
  } else {
    auto x = scheduler->spawn([n] { return fib(n - 1); });
    int y = fib(n - 2);
    return scheduler->steal(std::move(x)) + y;
  }
}
