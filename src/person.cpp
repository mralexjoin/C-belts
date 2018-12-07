#include <iostream>
#include <map>
#include <string>
#include <vector>

class Person{
public:
  void ChangeFirstName(const int& year, const std::string& first_name) {
    first_name_history[year] = first_name;
  }
  void ChangeLastName(const int& year, const std::string& last_name) {
    last_name_history[year] = last_name;
  }
  std::string GetFullName(int year) const {
    auto first_name_it = first_name_history.upper_bound(year);
    auto last_name_it = last_name_history.upper_bound(year);
    if (first_name_it == first_name_history.begin()
        && last_name_it == last_name_history.begin())
      return "Incognito";
    else if (first_name_it == first_name_history.begin())
      return prev(last_name_it)->second + " with unknown first name";
    else if (last_name_it == last_name_history.begin())
      return prev(first_name_it)->second + " with unknown last name";
    else
      return prev(first_name_it)->second + ' ' + prev(last_name_it)->second;
  }

private:
  std::map<int, std::string> first_name_history;
  std::map<int, std::string> last_name_history;
};
