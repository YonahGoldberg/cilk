#include <iostream>
#include <algorithm>
#include <iterator>
#include <functional>
#include "simple_scheduler.hpp"

SimpleScheduler scheduler;

int fib(int n) {
    if (n < 2) { 
        return (n);
    } else {
	    auto x = scheduler.spawn<int>([n] { return fib(n - 1); });
        int y = fib(n - 2);
        return (x.get() + y);
    }
}

void qsort(int* begin, int* end) {
  if (begin != end) {
    end--;
    int pivot = *end;
    auto middle = std::partition(begin, end, [pivot](int x) { return x < pivot; });
    std::swap(*end, *middle);

    auto x = scheduler.spawn<void>([begin, middle]() { qsort(begin, middle); });
    auto y = scheduler.spawn<void>([begin, middle, end]() mutable { qsort(++middle, ++end); });

    x.get();
    y.get();
  }
}

int main() {
    std::cout << "fib(10): " << fib(10) << std::endl;
    std::vector<int> nums = {5, 3, 4, 1, 2};
    qsort(nums.data(), nums.data() + nums.size());

    std::cout << "qsort(nums):";
    for (const auto& num : nums) {
        std::cout << " " << num;
    }
    std::cout << std::endl;
}
