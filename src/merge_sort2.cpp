#include <algorithm>
#include <iostream>
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

int main() {
  vector<int> v = {6, 4, 7, 6, 4, 4, 0, 1};
  MergeSort(begin(v), end(v));
  for (int x : v) {
    cout << x << " ";
  }
  cout << endl;
  return 0;
}
