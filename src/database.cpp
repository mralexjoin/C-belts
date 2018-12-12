#include "database.h"

void Database::Add(const Date& date, const std::string& event) {
  if (name_sorted_events.count(date) == 0 || name_sorted_events[date].count(event) == 0) {
    auto& name_sorted = name_sorted_events[date];
    auto& adding_sorted = adding_sorted_events[date];
    int max_index = 0;
    auto max_it = adding_sorted.rbegin();
    if (max_it != adding_sorted.rend()) {
      max_index = max_it->first + 1;
    }

    adding_sorted[max_index] = event;
    name_sorted[event] = max_index;
  }
}

void Database::Print(std::ostream& stream) const {
  for (const auto& [date, events] : adding_sorted_events) {
    for (const auto& [index, event] : events) {
      stream << date << ' ' << event << std::endl;
    }
  }
}

std::pair<Date, std::string> Database::Last(const Date& date) const {
  auto upper = adding_sorted_events.upper_bound(date);
  if (upper == adding_sorted_events.begin())
    throw std::invalid_argument("No entries");
  upper = prev(upper);

  return {upper->first, upper->second.rbegin()->second};
}

std::ostream& operator<<(std::ostream& stream, const std::pair<Date, std::string>& record) {
  return stream << record.first << ' ' << record.second;
}
