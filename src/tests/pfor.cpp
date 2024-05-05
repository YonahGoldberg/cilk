#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

#include "pfor.hpp"
#include "../scheduler_instance.hpp"

// Utility function for quicksort to ensure an array is sorted
bool _isSorted(const std::vector<int>& vec) {
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
void _assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "Assertion failed: " << message << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

int _quicksort(int *begin, int *end) {
  if (begin != end) {
    end--;
    int pivot = *end;
    auto middle =
        std::partition(begin, end, [pivot](int x) { return x < pivot; });
    std::swap(*end, *middle);

    _quicksort(begin, middle);
    _quicksort(++middle, ++end);
  }

  return 0;
}

int createAndSort() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(1, 1000); // Adjust range as needed

  std::vector<int> arr(100000);
  for (auto& elem : arr) {
      elem = dist(gen); // Fill the array with random integers
  }

  _quicksort(arr.data(), arr.data() + arr.size());
  _assertTrue(_isSorted(arr), "quicksort");

  return 0;
}

// Sort a random array x times
int pfor(int x) {
  for (int i = 0; i < x; i++) {
    scheduler->spawn([] { return createAndSort(); });
  }

  return 0;
}