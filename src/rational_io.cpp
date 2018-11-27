#include <iostream>
#include <sstream>
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

bool operator==(const Rational& lhs,
                const Rational& rhs) {
  return lhs.Numerator() == rhs.Numerator() && lhs.Denominator() == rhs.Denominator();
}

Rational operator+(const Rational& lhs,
                   const Rational& rhs) {
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
  return {lhs.Numerator() * rhs.Numerator(), lhs.Denominator() * rhs.Denominator()};
}

Rational operator/(const Rational& lhs, const Rational& rhs) {
  return {lhs.Numerator() * rhs.Denominator(), lhs.Denominator() * rhs.Numerator()};
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

int main() {
  {
    ostringstream output;
    output << Rational(-6, 8);
    if (output.str() != "-3/4") {
      cout << "Rational(-6, 8) should be written as \"-3/4\"" << endl;
      return 1;
    }
  }

  {
    istringstream input("5/7");
    Rational r;
    input >> r;
    bool equal = r == Rational(5, 7);
    if (!equal) {
      cout << "5/7 is incorrectly read as " << r << endl;
      return 2;
    }
  }

  {
    istringstream input("5/7 10/8");
    Rational r1, r2;
    input >> r1 >> r2;
    bool correct = r1 == Rational(5, 7) && r2 == Rational(5, 4);
    if (!correct) {
      cout << "Multiple values are read incorrectly: " << r1 << " " << r2 << endl;
      return 3;
    }

    input >> r1;
    input >> r2;
    correct = r1 == Rational(5, 7) && r2 == Rational(5, 4);
    if (!correct) {
      cout << "Read from empty stream shouldn't change arguments: " << r1 << " " << r2 << endl;
      return 4;
    }
  }

  cout << "OK" << endl;
  return 0;
}
