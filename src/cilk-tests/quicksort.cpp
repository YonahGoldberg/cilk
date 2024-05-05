#include <cilk/cilk.h>
#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

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

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 1000); // Adjust range as needed

    std::vector<int> arr(1000000);
    for (auto& elem : arr) {
        elem = dist(gen); // Fill the array with random integers
    }

    auto start = std::chrono::steady_clock::now();

    quicksort(arr.data(), arr.data() + arr.size());

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Sorting time: " << duration.count() << " ms" << std::endl;

    assertTrue(isSorted(arr), "Quicksort");
    return 0;
}