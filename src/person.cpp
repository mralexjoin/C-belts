#include <map>
#include <string>
#include <vector>

class Person{
public:
  Person() {
    birth_year = 0;
  }
  Person(const std::string& first_name,
         const std::string& last_name,
         const int& person_birth_year) {
    birth_year = person_birth_year;
    ChangeFirstName(birth_year, first_name);
    ChangeLastName(birth_year, last_name);
  }
  void ChangeFirstName(const int& year, const std::string& first_name) {
    if (year >= birth_year)
      first_name_history[year] = first_name;
  }
  void ChangeLastName(const int& year, const std::string& last_name) {
    if (year >= birth_year)
      last_name_history[year] = last_name;
  }
  std::string GetFullName(int year) const {
    return GetFullNameOptionalHistory(year, false);
  }
  std::string GetFullNameWithHistory(int year) const {
    return GetFullNameOptionalHistory(year, true);
  }
private:
  int birth_year;
  std::map<int, std::string> first_name_history;
  std::map<int, std::string> last_name_history;
  std::string GetFullNameOptionalHistory(const int& year,
                                         const bool& print_history) const {
    if (year < birth_year)
      return "No person";

    std::string first_name = GetName(first_name_history, year, print_history);
    std::string last_name = GetName(last_name_history, year, print_history);

    return first_name + ' ' + last_name;
  }


  std::string GetName(const std::map<int, std::string>& name_history,
                      const int& year,
                      const bool& print_history) const {
    std::vector<std::string> history;
    std::string name;
    for (const auto& [history_year, history_name] : name_history) {
      if (history_year > year)
        break;
      if (print_history
          && (history.size() == 0
              || history[history.size() - 1] != history_name))
        history.push_back(history_name);
      name = history_name;
    }

    if (history.size() > 1) {
      std::string history_string;
      for (int i = history.size() - 2; i >= 0; i--) {
        if (!history_string.empty())
          history_string += ", ";
        history_string += history[i];
      }
      name += " (" + history_string + ")";
    }

    return name;
  }
};
