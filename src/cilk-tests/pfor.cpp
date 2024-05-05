#include <cilk/cilk.h>
#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

void quicksort(int *begin, int *end) {
  if (begin != end) {
    end--;
    int pivot = *end;
    auto middle =
        std::partition(begin, end, [pivot](int x) { return x < pivot; });
    std::swap(*end, *middle);

    cilk_spawn quicksort(begin, middle);
    quicksort(++middle, ++end);
  }
}

// Sort a random array x times
void pfor(int x) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(1, 1000); // Adjust range as needed

  std::vector<int> arr(100000);
  for (auto& elem : arr) {
      elem = dist(gen); // Fill the array with random integers
  }
  std::vector<int> copy(arr);
  for (int i = 0; i < x; i++) {
    cilk_spawn quicksort(arr.data(), arr.data() + arr.size());
    arr = copy;
  }
  cilk_sync;
}

int main() {
  auto start = std::chrono::steady_clock::now();
  pfor(500);
  auto end = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PFor time: " << duration.count() << " ms" << std::endl;

  return 0;
}