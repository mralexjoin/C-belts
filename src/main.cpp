#include <iostream>
#include <map>
#include <string>
#include "get_ref_strict.cpp"

using namespace std;

int main() {
  map<int, string> m = {{0, "value"}};
  string& item = GetRefStrict(m, 0);
  item = "newvalue";
  cout << m[0] << endl; // выведет newvalue

  return 0;
}
