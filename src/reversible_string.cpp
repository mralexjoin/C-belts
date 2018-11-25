#include <algorithm>
#include <string>

class ReversibleString {
public:
  ReversibleString() {}
  ReversibleString(const std::string& _s) {
    s = _s;
  }
  void Reverse() {
    reverse(begin(s), end(s));
  }
  std::string ToString() const {
    return s;
  }
private:
  std::string s;
};
