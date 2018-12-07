#include <algorithm>
#include <iostream>
#include <set>

using namespace std;

set<int>::const_iterator FindNearestElement(
                                            const set<int>& numbers,
                                            int border) {

  auto equal_range = numbers.equal_range(border);
  if (equal_range.second == numbers.begin()
      || *equal_range.first == border)
    return equal_range.first;
  if (equal_range.first-- == numbers.end()) {
    return equal_range.first;
  }

  if (*equal_range.second - border < border - *equal_range.first)
    return equal_range.second;
  return equal_range.first;
}

int main() {
  set<int> numbers = {1, 4, 6};
  cout <<
    *FindNearestElement(numbers, 0) << " " <<
    *FindNearestElement(numbers, 3) << " " <<
    *FindNearestElement(numbers, 5) << " " <<
    *FindNearestElement(numbers, 6) << " " <<
    *FindNearestElement(numbers, 100) << endl;

  set<int> empty_set;

  cout << (FindNearestElement(empty_set, 8) == end(empty_set)) << endl;
  return 0;
}
