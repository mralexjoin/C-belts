#pragma once

#include <exception>
#include <iomanip>
#include <istream>
#include <ostream>
#include <sstream>

class Date {
 public:
  Date() {}
  Date(const int& year, const int& month, const int& day);
  inline int GetYear() const {return year;}
  inline int GetMonth() const {return month;}
  inline int GetDay() const {return day;}
 private:
  const int year = 0;
  const int month = 0;
  const int day = 0;
};

bool operator<(const Date& lhs, const Date& rhs);
inline bool operator<=(const Date& lhs, const Date& rhs) {return !(rhs < lhs);}
inline bool operator>(const Date& lhs, const Date& rhs) {return !(lhs <= rhs);}
inline bool operator>=(const Date& lhs, const Date& rhs) {return !(lhs < rhs);}
inline bool operator!=(const Date& lhs, const Date& rhs) {return (lhs < rhs || lhs > rhs);}
inline bool operator==(const Date& lhs, const Date& rhs) {return !(lhs != rhs);}

void CheckIfMinus(std::istream& stream);

int ReadInt(std::istream& stream);

std::ostream& operator<<(std::ostream& stream, const Date& date);

Date ParseDate(std::istream& stream);
