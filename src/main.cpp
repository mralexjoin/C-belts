#include <exception>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
// Реализуйте функции и методы классов и при необходимости добавьте свои

class Date {
public:
  Date() {}
  Date(const int& new_year, const int& new_month, const int& new_day) {
    if (new_month < 1 || new_month > 12)
      throw std::invalid_argument("Month value is invalid: " + std::to_string(new_month));
    if (new_day < 1 || new_day > 31)
      throw std::invalid_argument("Day value is invalid: " + std::to_string(new_day));
    year = new_year;
    month = new_month;
    day = new_day;
  }
  int GetYear() const {
    return year;
  }
  int GetMonth() const {
    return month;
  }
  int GetDay() const {
    return day;
  }
private:
  int year = 0;
  int month = 0;
  int day = 0;
};

bool operator<(const Date& lhs, const Date& rhs) {
  if (lhs.GetYear() == rhs.GetYear()) {
    if (lhs.GetMonth() == rhs.GetMonth()) {
      return lhs.GetDay() < rhs.GetDay();
    }
    return lhs.GetMonth() < rhs.GetMonth();
  }
  return lhs.GetYear() < rhs.GetYear();
}

void CheckIfMinus(std::istream& stream) {
  char c;
  stream >> c;
  if (c != '-')
    throw std::runtime_error("");
}

int ReadInt(std::istream& stream) {
  int i;
  stream >> i;
  if (stream.fail())
    throw std::runtime_error("");
  return i;
}

std::istream& operator>>(std::istream& stream, Date& date) {
  std::string string_date;
  stream >> string_date;
  std::stringstream sstream(string_date);
  int year, month, day;
  try {
    year = ReadInt(sstream);
    CheckIfMinus(sstream);
    month = ReadInt(sstream);
    CheckIfMinus(sstream);
    day = ReadInt(sstream);
    if (!sstream.eof())
      throw std::runtime_error("");
    date = {year, month, day};
  }
  catch (const std::runtime_error& ex) {
    throw std::runtime_error("Wrong date format: " + string_date);
  }
  return stream;
}

std::ostream& operator<<(std::ostream& stream, const Date& date) {
  return stream << std::setfill('0')
                << std::setw(4) << date.GetYear() << '-'
                << std::setw(2) << date.GetMonth() << '-'
                << std::setw(2) << date.GetDay();
}

class Database {
public:
  void AddEvent(const Date& date, const std::string& event) {
    if (events.count(date) == 0 || events[date].count(event) == 0)
      events[date].insert(event);
  }
  bool DeleteEvent(const Date& date, const std::string& event) {
    if (events.count(date) == 0)
      return false;
    if (events[date].count(event) == 0)
      return false;
    events[date].erase(event);
    if (events[date].empty())
      events.erase(date);
    return true;
  }
  int DeleteDate(const Date& date) {
    if (events.count(date) == 0)
      return 0;
    int size = events[date].size();
    events.erase(date);
    return size;
  }

  std::set<std::string> Find(const Date& date) const {
    if (events.count(date) == 0)
      return {};
    return events.at(date);
  }

  void Print() const {
    for (const auto& [date, day_events] : events) {
      for (const auto& event : day_events) {
        std::cout << date << ' ' << event << std::endl;
      }
    }
  }
private:
  std::map<Date, std::set<std::string>> events;
};

int main() {
  Database db;

  std::string command;
  while (getline(std::cin, command)) {
    // Считайте команды с потока ввода и обработайте каждую
    std::stringstream stream(command);
    std::string operation;
    try {
      if (stream >> operation) {
        if (operation == "Add") {
          Date date;
          std::string event;
          stream >> date >> event;
          db.AddEvent(date, event);
        }
        else if (operation == "Del") {
          Date date;
          stream >> date;
          std::string event;
          if (stream >> event) {
            if (db.DeleteEvent(date, event))
              std::cout << "Deleted successfully" << std::endl;
            else
              std::cout << "Event not found" << std::endl;
          }
          else {
            std::cout << "Deleted " << db.DeleteDate(date)
                      << " events" << std::endl;
          }
        }
        else if (operation == "Find") {
          Date date;
          stream >> date;
          std::set<std::string> events = db.Find(date);
          for (const auto& event : events)
            std::cout << event << std::endl;
        }
        else if (operation == "Print") {
          db.Print();
        }
        else {
          std::cout << "Unknown command: " << operation << std::endl;
        }
      }

    }
    catch (const std::exception& ex) {
      std::cout << ex.what() << std::endl;
    }
  }

  return 0;
}
