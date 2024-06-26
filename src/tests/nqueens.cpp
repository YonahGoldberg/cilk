#include "nqueens.hpp"
#include "../scheduler_instance.hpp"

int ok(int n, char *a) {
  int i, j;
  char p, q;

  for (i = 0; i < n; i++) {
    p = a[i];
    for (j = i + 1; j < n; j++) {
      q = a[j];
      if (q == p || q == p - (j - i) || q == p + (j - i))
        return 0;
    }
  }

  return 1;
}

int nqueens(int n, int j, char *a) {
  if (n == j) {
    return 1;
  }

  std::vector<std::future<int>> futures;
  int solNum = 0;

  for (int i = 0; i < n; i++) {
    char *b = (char *)alloca((j + 1) * sizeof(char));
    memcpy(b, a, j * sizeof(char));
    b[j] = i;

    if (ok(j + 1, b)) {
      // Spawn a new task for exploring this partial solution
      auto fut = scheduler->spawn(
          [n, j, b]() mutable { return nqueens(n, j + 1, b); });
      futures.push_back(std::move(fut));
    }
  }

  for (auto &fut : futures) {
    solNum += scheduler->sync(std::move(fut));
  }

  return solNum;
}