#include "date.h"

Date::Date(const int& year, const int& month, const int& day) :
  year(year),
  month(month),
  day(day) {
  if (month < 1 || month > 12)
    throw std::invalid_argument("Month value is invalid: " + std::to_string(month));
  if (day < 1 || day > 31)
    throw std::invalid_argument("Day value is invalid: " + std::to_string(day));
}

bool operator<(const Date& lhs, const Date& rhs) {
  if (lhs.GetYear() == rhs.GetYear()) {
    if (lhs.GetMonth() == rhs.GetMonth())
      return lhs.GetDay() < rhs.GetDay();
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

std::ostream& operator<<(std::ostream& stream, const Date& date) {
  return stream << std::setfill('0')
                << std::setw(4) << date.GetYear() << '-'
                << std::setw(2) << date.GetMonth() << '-'
                << std::setw(2) << date.GetDay();
}

Date ParseDate(std::istream& stream) {
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
    return {year, month, day};
  }
  catch (const std::runtime_error& ex) {
    throw std::runtime_error("Wrong date format: " + string_date);
  }
}
