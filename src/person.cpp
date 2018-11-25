#include <map>
#include <set>
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
    return GetFullNameOptionalHistory(year, false);
  }
  std::string GetFullNameWithHistory(int year) {
    return GetFullNameOptionalHistory(year, true);
  }
private:
  std::map<int, std::string> first_name_history;
  std::map<int, std::string> last_name_history;
  std::string GetFullNameOptionalHistory(int year, bool history) {
    auto first_name_it = first_name_history.upper_bound(year);
    auto last_name_it = last_name_history.upper_bound(year);

    if (first_name_it == first_name_history.begin()
        && last_name_it == last_name_history.begin())
      return "Incognito";
    else if (first_name_it == first_name_history.begin())
      return GetName(last_name_it, last_name_history, history)
        + " with unknown first name";
    else if (last_name_it == last_name_history.begin())
      return GetName(first_name_it, first_name_history, history)
        + " with unknown last name";
    else
      return GetName(first_name_it, first_name_history, history)
        + ' '
        + GetName(last_name_it, last_name_history, history);
  }
  std::string GetName(std::map<int, std::string>::iterator it,
                      const std::map<int, std::string>& m,
                      bool out_history) {
    std::string name = (--it)->second;
    if (out_history) {
      std::string history = GetHistory(it, m);
      if (history.size() > 0)
        name += ' ' + history;
    }
    return name;
  }
  std::string GetHistory(std::map<int, std::string>::iterator it,
                         const std::map<int, std::string>& m) {
    std::set<std::string> printed;
    std::string history = "";
    std::string current_name = it->second;

    while (it-- != m.begin()) {
      if (printed.count(it->second) == 0) {
        if (printed.size() == 0 && it->second != current_name) {
          history += it->second;
          printed.insert(it->second);
        }
        else if (printed.size() != 0) {
          history += ", " + it->second;
          printed.insert(it->second);
        }
      }
    }

    if (printed.size() > 0)
      return "(" + history + ")";
    return "";
  }
};
