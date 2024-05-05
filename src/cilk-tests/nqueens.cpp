#include <algorithm>
#include <chrono>
#include <cilk/cilk.h>
#include <iostream>

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

  int solNum = 0;
  std::vector<int> count(n);

  for (int i = 0; i < n; i++) {
    char *b = (char *)alloca((j + 1) * sizeof(char));
    memcpy(b, a, j * sizeof(char));
    b[j] = i;

    if (ok(j + 1, b)) {
      // Spawn a new task for exploring this partial solution
      count[i] = cilk_spawn nqueens(n, j + 1, b);
    }
  }
  cilk_sync;

  for (int i = 0; i < n; i++) {
    solNum += count[i];
  }

  return solNum;
}

int main() {
  int n = 14;
  char *a = new char[n];
  char *copy = new char[n];
  std::copy(a, a + n, copy);

  auto start = std::chrono::steady_clock::now();
  nqueens(n, 0, a);
  auto end = std::chrono::steady_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "NQueens time: " << duration.count() << " ms" << std::endl;
}