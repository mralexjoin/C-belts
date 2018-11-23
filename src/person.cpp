#include <map>
#include <string>

class Person{
public:
  void ChangeFirstName(int year, const std::string& first_name) {
    first_name_history[year] = first_name;
  }
  void ChangeLastName(int year, const std::string& last_name) {
    last_name_history[year] = last_name;
  }
  std::string GetFullName(int year) {
    auto first_name_it = first_name_history.upper_bound(year);
    auto last_name_it = last_name_history.upper_bound(year);

    if (first_name_it == first_name_history.begin()
        && last_name_it == last_name_history.begin())
      return "Incognito";
    else if (first_name_it == first_name_history.begin())
      return (--last_name_it)->second + " with unknown first name";
    else if (last_name_it == last_name_history.begin())
      return (--first_name_it)->second + " with unknown last name";
    return (--first_name_it)->second + ' ' + (--last_name_it)->second;
  }
private:
  std::map<int, std::string> first_name_history;
  std::map<int, std::string> last_name_history;
  std::map<int, std::string>::iterator
  GreatestLess(std::map<int, std::string>& m, const int& value) {
    auto it = m.upper_bound(value);
    if (it == m.begin())
      return m.end();
    return --it;
  }
};
