#include <iostream>
#include <exception>
using namespace std;

class Rational {
public:
  Rational() {
    // Реализуйте конструктор по умолчанию
    numerator = 0;
    denominator = 1;
  }

  Rational(int new_numerator, int new_denominator) {
    // Реализуйте конструктор
    if (new_denominator == 0)
      throw invalid_argument("Invalid argument");

    bool negative = (new_numerator < 0 && new_denominator > 0
                     || new_numerator > 0 && new_denominator < 0);
    int positive_numerator = abs(new_numerator);
    int positive_denominator = abs(new_denominator);
    int s = scf(positive_numerator, positive_denominator);
    numerator = positive_numerator / s;
    denominator = positive_denominator / s;
    if (negative)
      numerator = -numerator;
  }

  int Numerator() const {
    // Реализуйте этот метод
    return numerator;
  }

  int Denominator() const {
    // Реализуйте этот метод
    return denominator;
  }

private:
  // Добавьте поля
  int numerator;
  int denominator;
  int scf(int a, int b) const {
    while (a > 0 && b > 0) {
      if (a > b)
        a %= b;
      else
        b %= a;
    }
    if (a != 0)
      return a;
    return b;
  }
};

// Реализуйте для класса Rational операторы ==, + и -
bool operator==(const Rational& lhs, const Rational& rhs) {
  return lhs.Numerator() == rhs.Numerator() && lhs.Denominator() == rhs.Denominator();
}

Rational operator+(const Rational& lhs, const Rational& rhs) {
  if (lhs.Denominator() == rhs.Denominator())
    return {lhs.Numerator() + rhs.Numerator(), lhs.Denominator()};
  int l_numerator = lhs.Numerator() * rhs.Denominator();
  int r_numerator = rhs.Numerator() * lhs.Denominator();
  int denominator = lhs.Denominator() * rhs.Denominator();
  return {l_numerator + r_numerator, denominator};
}

Rational operator-(const Rational& lhs,
                   const Rational& rhs) {
  return lhs + Rational(-rhs.Numerator(), rhs.Denominator());
}

// Реализуйте для класса Rational операторы * и /
Rational operator*(const Rational& lhs, const Rational& rhs) {
  return {lhs.Numerator() * rhs.Numerator(),
          lhs.Denominator() * rhs.Denominator()};
}

Rational operator/(const Rational& lhs, const Rational& rhs) {
  if (rhs.Numerator() == 0)
    throw domain_error("Division by zero");
  return {lhs.Numerator() * rhs.Denominator(),
          lhs.Denominator() * rhs.Numerator()};
}

// Реализуйте для класса Rational операторы << и >>
istream& operator>>(istream& stream, Rational& r) {
  int numerator, denominator;
  if (stream >> numerator) {
    stream.ignore(1);
    if (stream >> denominator) {
      r = {numerator, denominator};
    }
  }
  return stream;
}

ostream& operator<<(ostream& stream, const Rational& r) {
  stream << r.Numerator() << '/' << r.Denominator();
  return stream;
}

// Реализуйте для класса Rational оператор(ы), необходимые для использования его
// в качестве ключа map'а и элемента set'а
bool operator<(const Rational& lhs, const Rational& rhs) {
  if (lhs.Denominator() == rhs.Denominator())
    return lhs.Numerator() < rhs.Numerator();
  int l_numerator = lhs.Numerator() * rhs.Denominator();
  int r_numerator = rhs.Numerator() * lhs.Denominator();
  return l_numerator < r_numerator;
}

int main() {
  Rational lhs, rhs, result;
  char operation;
  cin >> lhs >> operation >> rhs;
  try {
    switch (operation) {
    case '+':
      result = lhs + rhs;
      break;
    case '-':
      result = lhs - rhs;
      break;
    case '*':
      result = lhs * rhs;
      break;
    case '/':
      result = lhs / rhs;
      break;
    }
    cout << result << endl;
  }
  catch (exception& ex) {
    cout << ex.what() << endl;
  }
  return 0;
}
