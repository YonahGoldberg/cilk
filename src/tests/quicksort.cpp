#include "quicksort.hpp"
#include "../scheduler_instance.hpp"

int seqQuicksort(int *begin, int *end) {
  if (begin != end) {
    end--;
    int pivot = *end;
    auto middle =
        std::partition(begin, end, [pivot](int x) { return x < pivot; });
    std::swap(*end, *middle);

    seqQuicksort(begin, middle);
    seqQuicksort(++middle, ++end);
  }

  return 0;
}

int quicksort(int *begin, int *end) {
  if (end - begin <= 5000) {
    seqQuicksort(begin, end);
    return 0;
  }

  end--;
  int pivot = *end;
  auto middle =
      std::partition(begin, end, [pivot](int x) { return x < pivot; });
  std::swap(*end, *middle);

  auto x =
      scheduler->spawn([begin, middle]() { return quicksort(begin, middle); });
  quicksort(++middle, ++end);

  scheduler->sync(std::move(x));

  return 0;
}