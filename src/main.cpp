#include <algorithm>
#include <iostream>
#include <vector>

template<typename ForwardIterator, typename UnaryPredicate>
ForwardIterator max_element_if(ForwardIterator first,
                               ForwardIterator last,
                               UnaryPredicate pred) {
  auto max_it = std::find_if(first, last, pred);
  for (auto it = max_it; it != last; it++) {
    if (*max_it < *it && pred(*it)) {
      max_it = it;
    }
  }
  return max_it;
}

int main() {
  std::vector<int> v = {2, 3, 4, 5, 5};
  auto it = max_element_if(v.begin(), v.end(), [](const int& x) {
                                                    return x < 2;});
  if (it == v.end())
    std::cout << "End" << std::endl;
  else
    std::cout << *it << std::endl;
  return 0;
}
