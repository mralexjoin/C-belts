#include <map>
#include <string>
#include <vector>

class SortedStrings {
public:
  void AddString(const std::string& s) {
    m[s]++;
  }
  std::vector<std::string> GetSortedStrings() {
    std::vector<std::string> v;
    for (const auto& e : m)
      for (int i = 0; i < e.second; i++)
        v.push_back(e.first);
    return v;
  }
private:
  std::map<std::string, size_t> m;
};
