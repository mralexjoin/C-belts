#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <stdexcept>
  
using namespace std;
  
enum class Gender {
  MALE,
  FEMALE,
  UNKNOWN
};

ostream& operator<<(ostream& out, const Gender& gender) {
  switch (gender) {
    case Gender::MALE:
      out << "MALE";
      break;
    case Gender::FEMALE:
      out << "FEMALE";
      break;
    case Gender::UNKNOWN:
      out << "UNKNOWN";
      break;
  }
  return out;
}
  
size_t index_in_bounds(int size, int index) {
  if (index < 0)
    return size + index;
  if (index >= size)
    return index - size;
  return index;
}
  
int main() {
  constexpr int table_size = 2004;
  array<Gender, table_size> table;
  table.fill(Gender::UNKNOWN);
  
  int diff = table_size;
  while (diff > 0) {
    diff = 0;
    for (int index = 0; index < table_size; index++) {
      Gender gender = Gender::MALE;
      for (int i : {2, 4}) {
        int left = index_in_bounds(table_size, index - i);
        int right = index_in_bounds(table_size, index + i);
        if (table[left] == Gender::MALE && table[right] == Gender::MALE) {
          gender = Gender::FEMALE;
        }
      }
      if (table[index] != gender) {
        table[index] = gender;
        diff++;
      }
    }
  }
  
  for (int i = 1994; i < 2004; i++) {
    cout << "i = " << i << ": " << table[i] << endl;
  }

  for (int i = 0; i < 16; i++) {
    cout << "i = " << i << ": " << table[i] << endl;
  }

  cout << count(table.begin(), table.end(), Gender::MALE) << endl;
  
  return 0;
}