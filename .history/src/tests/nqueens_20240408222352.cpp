#include "nqueens.hpp"
#include "scheduler_instance.hpp"

int ok (int n, char *a) {
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
    printf("HERE1\n");
    for (int i = 0; i < n; i++) {
        auto b = std::make_unique<char[]>(j + 1);
        printf("HERENOW\n");
        memcpy(b.get(), a, j * sizeof(char));
        printf("HERENOW3\n");
        b[j] = i;

        if (ok(j + 1, b.get())) {
            // Spawn a new task for exploring this partial solution
            auto fut = scheduler->spawn([n, j, &b]() mutable {
                return nqueens(n, j + 1, b.get());
            });
            futures.push_back(std::move(fut));
        }
    }
    
    // printf("HERE2\n");

    // Collect results from all futures
    for (auto &fut : futures) {
        solNum += fut.get();
    }
    printf("RETURN\n");
    return solNum;
}
