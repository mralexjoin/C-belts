#pragma once

#include <algorithm>
#include <map>
#include <ostream>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "date.h"

class Database {
 public:
  void Add(const Date& date, const std::string& event);

  template<typename Predicate>
  size_t RemoveIf(const Predicate& predicate) {
    std::vector<std::pair<Date, std::string>> to_delete_by_name;
    std::vector<std::pair<Date, int>> to_delete_by_index;
    for (const auto& [date, events] : name_sorted_events) {
      for (const auto& [event, index] : events) {
        if (predicate(date, event)) {
          to_delete_by_name.push_back({date, event});
          to_delete_by_index.push_back({date, index});
        }
      }
    }

    for (const auto& [date, event] : to_delete_by_name) {
      auto& events = name_sorted_events[date];
      events.erase(event);
      if (events.empty()) {
        name_sorted_events.erase(date);
        adding_sorted_events.erase(date);
      }
    }

    for (const auto& [date, index] : to_delete_by_index) {
      if (adding_sorted_events.count(date) != 0) {
        adding_sorted_events[date].erase(index);
      }
    }

    return to_delete_by_name.size();
  };

  template<typename Predicate>
  std::vector<std::pair<Date, std::string>> FindIf(const Predicate& predicate) const {
    std::vector<std::pair<Date, std::string>> found_records;
    for (const auto& [date, events] : adding_sorted_events) {
      for (const auto& [index, event] : events) {
        if (predicate(date, event)) {
          found_records.push_back({date, event});
        }
      }
    }
    return found_records;
  }

  void Print(std::ostream&) const;

  std::pair<Date, std::string> Last(const Date&) const;

 private:
  std::map<Date, std::map<std::string, int>> name_sorted_events;
  std::map<Date, std::map<int, std::string>> adding_sorted_events;
};

std::ostream& operator<<(std::ostream&, const std::pair<Date, std::string>&);
