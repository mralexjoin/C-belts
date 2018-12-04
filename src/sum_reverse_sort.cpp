#include <algorithm>
#include "sum_reverse_sort.h"

int Sum(int x, int y) {
  return x + y;
}

string Reverse(string s) {
  size_t half = s.size() / 2;
  for (size_t left = 0; left < half; left++) {
    size_t right = s.size() - left - 1;
    char tmp = s[left];
    s[left] = s[right];
    s[right] = tmp;
  }
  return s;
}
void Sort(vector<int>& nums) {
  sort(begin(nums), end(nums));
}
