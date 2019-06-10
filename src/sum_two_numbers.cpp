#include <cstdint>
#include <iostream>
#include <limits>

int main() {
  int64_t x, y;
  std::cin >> x >> y;
  if (y > x) {
    std::swap(x, y);
  }
  if ((x > 0 && y > std::numeric_limits<int64_t>::max() - x)
      || (x < 0 && y < std::numeric_limits<int64_t>::min() - x)) {
    std::cout << "Overflow!\n";
  }
  else {
    std::cout << x + y << "\n";
  }
  return 0;
}
