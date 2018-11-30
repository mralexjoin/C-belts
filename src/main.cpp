#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <vector>

int main() {
  int n;
  std::cin >> n;
  std::vector<int> v(n);
  for (auto& i : v)
    std::cin >> i;
  int64_t sum = 0;
  sum = std::accumulate(begin(v), end(v), sum);
  const int64_t avg = sum / static_cast<int64_t>(v.size());
  std::vector<size_t> indices;
  for (size_t i = 0; i < v.size(); i++) {
    if (v[i] > avg)
      indices.push_back(i);
  }

  std::cout << indices.size() << std::endl;
  for (const auto& i : indices) {
    std::cout << i << ' ';
  }
  std::cout << std::endl;

  return 0;
}
