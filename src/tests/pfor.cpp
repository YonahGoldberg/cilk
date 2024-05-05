#include "pfor.hpp"
#include "../scheduler_instance.hpp"

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>

int quicksort1(int *begin, int *end) {
  if (begin != end) {
    end--;
    int pivot = *end;
    auto middle =
        std::partition(begin, end, [pivot](int x) { return x < pivot; });
    std::swap(*end, *middle);

    quicksort1(begin, middle);
    quicksort1(++middle, ++end);
  }

  return 0;
}

// Sort a random array x times
int pfor(int x) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(1, 1000); // Adjust range as needed

  std::vector<int> arr(100000);
  for (auto& elem : arr) {
      elem = dist(gen); // Fill the array with random integers
  } 
  std::vector<int> copy(arr);

  for (int i = 0; i < x; i++) {
     auto x = scheduler->spawn([&arr] {
        return quicksort1(arr.data(), arr.data() + arr.size());
    });
    scheduler->sync(std::move(x));
    arr = copy;
  }

  return 0;
}