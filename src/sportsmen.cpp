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
    auto it = std::find(athletes.begin(), athletes.end(), follower);
    if (it == athletes.end()) {
      athletes.push_front(current);
    }
    else {
      athletes.insert_after(it, current);
    }
  }

  athletes.reverse();

  for (const size_t athlete : athletes) {
    std::cout << athlete << ' ';
  }
  std::cout << '\n';

  return 0;
}
