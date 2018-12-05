#include <iostream>
#include <set>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

template <typename T>
std::vector<T> FindGreaterElements(const std::set<T>& elements, const T& border) {
  auto greater_iterator = find_if(begin(elements), end(elements),
                                  [&border](const T& element){
                                    return element > border;
                                  });
  std::vector<T> greater_elements;
  while (greater_iterator != end(elements))
    greater_elements.push_back(*(greater_iterator++));
  return greater_elements;
}

int main() {
  for (int x : FindGreaterElements(set<int>{1, 5, 7, 8}, 5)) {
    cout << x << " ";
  }
  cout << endl;

  string to_find = "Python";
  cout << FindGreaterElements(set<string>{"C", "C++"}, to_find).size() << endl;
  return 0;
}
