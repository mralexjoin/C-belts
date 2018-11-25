#include <string>

class ReversibleString {
public:
  ReversibleString() {}
  ReversibleString(const std::string& _s) {
    s = _s;
  }
  void Reverse() {
    size_t upper_bound = s.size() - 1;
    for (size_t i = 0; i < s.size() / 2; i++) {
      size_t left = i, right = upper_bound - i;
      char tmp = s[left];
      s[left] = s[right];
      s[right] = tmp;
    }
  }
  std::string ToString() const {
    return s;
  }
private:
  std::string s;
};
