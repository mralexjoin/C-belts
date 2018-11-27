#include <exception>
#include <iostream>
#include "ensure_equal.cpp"

int main() {
  try {
    EnsureEqual("f", "f");
  }
  catch (std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }

  return 0;
}
