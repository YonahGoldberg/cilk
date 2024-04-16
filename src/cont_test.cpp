#include "simple_cont_scheduler.hpp"
#include <random>

SimpleContScheduler<void> scheduler;

void test() {
  for (int i = 0; i < 10; i++) {
    scheduler.spawn(
        [i] { std::cout << "hello from iteration " << i << std::endl; });
  }
}

void quicksort(int *begin, int *end) {
  if (begin != end) {
    end--;
    int pivot = *end;
    auto middle =
        std::partition(begin, end, [pivot](int x) { return x < pivot; });
    std::swap(*end, *middle);

    scheduler.spawn([begin, middle]() { quicksort(begin, middle); });
    quicksort(++middle, ++end);
  }
}

int main() {
  scheduler.run([] {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 1000); // Adjust range as needed

    std::vector<int> arr(10000);
    for (auto &elem : arr) {
      elem = dist(gen); // Fill the array with random integers
    }

    quicksort(arr.data(), arr.data() + arr.size());

    for (auto &elem : arr) {
      std::cout << elem << std::endl;
    }
  });
  // scheduler.run(test);
}