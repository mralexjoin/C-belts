#include <cstdint>
#include <iostream>

int main() {
  size_t n;
  std::cin >> n;

  uint16_t r;
  std::cin >> r;

  uint64_t mass = 0;
  for (size_t i = 0; i < n; i++) {
    uint64_t w, h, d;
    std::cin >> w >> h >> d;
    mass += w * h * d * r;
  }

  std::cout << mass << std::endl;

  return 0;
}
