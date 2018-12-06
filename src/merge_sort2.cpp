#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
  if (range_end - range_begin <= 1)
    return;
  vector<typename RandomIt::value_type> elements(range_begin, range_end);
  auto elements_middle = begin(elements) + elements.size() / 2;
  MergeSort(begin(elements), elements_middle);
  MergeSort(elements_middle, end(elements));
  merge(begin(elements), elements_middle, elements_middle, end(elements), range_begin);
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
