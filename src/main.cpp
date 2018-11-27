#include <iostream>
#include <string>
#include <vector>

struct Student {
public:
  Student(std::istream& stream) {
    stream >> first_name >> last_name
           >> day >> month >> year;
  }
  void PrintFullName(std::ostream& stream) const {
    stream << first_name << ' ' << last_name << std::endl;
  }
  void PrintBirthDate(std::ostream& stream) const {
    stream << day << '.' << month << '.' << year << std::endl;
  }
  std::string first_name;
  std::string last_name;
  int day;
  int month;
  int year;
};

int main() {
  size_t n;
  std::cin >> n;

  std::vector<Student> students;
  for (size_t i = 0; i < n; i++) {
    students.push_back({std::cin});
  }

  size_t m;
  std::cin >> m;
  for (size_t i = 0; i < m; i++) {
    std::string request;
    std::cin >> request;

    int k;
    std::cin >> k;

    if (k < 1 || k > students.size()) {
      std::cout << "bad request" << std::endl;
    }
    else {
      k--;
      if (request == "name")
        students[k].PrintFullName(std::cout);
      else if (request == "date")
        students[k].PrintBirthDate(std::cout);
      else
        std::cout << "bad request" << std::endl;
    }
  }

  return 0;
}
