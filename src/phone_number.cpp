#include <sstream>
#include <stdexcept>

#include "phone_number.h"

string ReadToMinus(stringstream& stream, string part_name) {
  string s;
  char c;
  while ((stream >> c) && c != '-')
    s += c;
  if (s.empty())
    throw invalid_argument("Empty " + part_name);
  return s;
}

PhoneNumber::PhoneNumber(const string &international_number) {
  stringstream ss(international_number);
  char c;
  ss >> c;
  if (c != '+')
    throw invalid_argument("Number should start with \"+\"");

  country_code_ = ReadToMinus(ss, "country code");
  city_code_ = ReadToMinus(ss, "city code");
  ss >> local_number_;
  if (local_number_.empty())
    throw invalid_argument("Empty local number");
}

string PhoneNumber::GetCountryCode() const {
  return country_code_;
}

string PhoneNumber::GetCityCode() const {
  return city_code_;
}

string PhoneNumber::GetLocalNumber() const {
  return local_number_;
}

string PhoneNumber::GetInternationalNumber() const {
  return '+' + country_code_ + '-' + city_code_ + '-' + local_number_;
}
