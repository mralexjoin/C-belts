#include "test_runner.h"
#include <algorithm>
#include <memory>
#include <vector>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
  if (range_end - range_begin <= 2)
    return;
  vector<typename RandomIt::value_type> elements(range_begin, range_end);
  int part = elements.size() / 3;
  auto end_part_one = begin(elements) + part;
  auto end_part_two = end_part_one + part;
  MergeSort(begin(elements), end_part_one);
  MergeSort(end_part_one, end_part_two);
  MergeSort(end_part_two, end(elements));
  vector<typename RandomIt::value_type> tmp;
  merge(begin(elements), end_part_one, end_part_one, end_part_two, back_inserter(tmp));
  merge(begin(tmp), end(tmp), end_part_two, end(elements), range_begin);
}

void TestIntVector() {
  vector<int> numbers = {6, 1, 3, 9, 1, 9, 8, 12, 1};
  MergeSort(begin(numbers), end(numbers));
  ASSERT(is_sorted(begin(numbers), end(numbers)));
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestIntVector);
  return 0;
}
