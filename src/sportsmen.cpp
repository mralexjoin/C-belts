#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

int main() {
  const size_t SIZE = 100000;

  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  size_t n;
  std::cin >> n;

  std::map<size_t, size_t> indices_by_athletes;
  std::map<size_t, size_t> athletes_by_indices;

  for (size_t i = 0; i < n; i++) {
    size_t current, next;
    std::cin >> current >> next;

    size_t follower_index = SIZE;
    if (indices_by_athletes.count(next) != 0) {
      follower_index = indices_by_athletes.at(next);
    }
  }

  for (const int athlete : athletes) {
    std::cout << athlete << ' ';
  }
  std::cout << '\n';

  return 0;
}
