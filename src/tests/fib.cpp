#include "fib.hpp"
#include "../scheduler_instance.hpp"

int fibSeq(int n) {
  if (n < 2) {
    return n;
  } else {
    return fibSeq(n - 1) + fibSeq(n - 2);
  }
}

int fib(int n) {
  if (n < 35) {
    return fibSeq(n);
  } else {
    auto x = scheduler->spawn([n] { return fib(n - 1); });
    int y = fib(n - 2);
    return scheduler->sync(std::move(x)) + y;
  }
}
