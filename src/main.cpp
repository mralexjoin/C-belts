#include <iostream>
#include "matrix.cpp"

int main() {
  Matrix one;
  Matrix two;

  std::cin >> one >> two;
  std::cout << one + two << std::endl;
  return 0;
}
