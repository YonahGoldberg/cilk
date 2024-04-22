#include "simple_cont_scheduler.hpp"
#include "simple_cs_scheduler.hpp"
#include <random>

void assert_true(bool cond) {
  if (!cond) {
    std::cout << "Assertion failed" << std::endl;
    exit(1);
  }
}

SimpleContScheduler<void> scheduler;
// SimpleCSScheduler<void> scheduler;

void quicksort(int* begin, int* end);

bool isSorted(const std::vector<int>& vec) {
    for (size_t i = 0; i < vec.size() - 1; i++) {
        if (vec[i] > vec[i + 1]) {
            return false;
        }
    }
    return true;
}

void manyQuicksort() {
  for (int i = 0; i < 50; i++) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 1000);

    std::vector<int> arr(1000000);
    for (auto &elem : arr) {
      elem = dist(gen); // Fill the array with random integers
    }

    scheduler.spawn([arr] {
      quicksort((int*)arr.data(), (int*)arr.data() + arr.size());
      assert(isSorted(arr));
    });
  }
  scheduler.jump();
}

void quicksort(int* begin, int* end) {
  if (begin != end) {
    end--;
    int pivot = *end;
    auto middle =
        std::partition(begin, end, [pivot](int x) { return x < pivot; });
    std::swap(*end, *middle);

    quicksort(begin, middle);
    quicksort(++middle, ++end);
  }
}

void pquicksort(int *begin, int *end) {
  if (begin != end) {
    end--;
    int pivot = *end;
    auto middle =
        std::partition(begin, end, [pivot](int x) { return x < pivot; });
    std::swap(*end, *middle);

    if (end - begin < 5000) {
      quicksort(begin, middle);
      quicksort(++middle, ++end);
    } else {
      scheduler.spawn([begin, middle]() { pquicksort(begin, middle); });
      pquicksort(++middle, ++end);
    }
  }
  scheduler.jump();
}

int main() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(1, 1000);

  std::vector<int> arr1(1000000);
  for (auto &elem : arr1) {
    elem = dist(gen); // Fill the array with random integers
  }

  std::vector<int> arr2(arr1);

  auto startTime = std::chrono::high_resolution_clock::now();
  scheduler.run([&arr1] {
    pquicksort((int*)arr1.data(), (int*)arr1.data() + arr1.size());
  }, 8);

  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

  assert_true(isSorted(arr1));

  std::cout << "PQuicksort Elapsed time: " << duration << " milliseconds" << std::endl;

  startTime = std::chrono::high_resolution_clock::now();
  quicksort((int*)arr2.data(), (int*)arr2.data() + arr2.size());
  endTime = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

  assert_true(isSorted(arr2));

  std::cout << "Quicksort Elapsed time: " << duration << " milliseconds" << std::endl;
}