#include "quicksort.hpp"
#include "../scheduler_instance.hpp"

int quicksort(int *begin, int *end) {
  if (begin != end) {
    end--;
    int pivot = *end;
    auto middle =
        std::partition(begin, end, [pivot](int x) { return x < pivot; });
    std::swap(*end, *middle);

    if (end - begin <= 32) {
      quicksort(begin, middle);
      quicksort(++middle, ++end);
      return 0;
    }

    auto x = scheduler->spawn(
        [begin, middle]() { return quicksort(begin, middle); });
    quicksort(++middle, ++end);

    scheduler->steal(std::move(x));
  }

  return 0;
}