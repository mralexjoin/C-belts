#include <algorithm>
#include <iostream>
#include <forward_list>

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  size_t n;
  std::cin >> n;

  std::forward_list<size_t> athletes;
  for (size_t i = 0; i < n; i++) {
    size_t current, follower;
    std::cin >> current >> follower;
    if (follower == 0) {
      athletes.push_front(current);
    }
    else {
      athletes.insert_after(
                            std::find(athletes.begin(), athletes.end(), follower),
                            current);
    }
  }

  athletes.reverse();

  for (const int athlete : athletes) {
    std::cout << athlete << ' ';
  }
  std::cout << '\n';

  return 0;
}
