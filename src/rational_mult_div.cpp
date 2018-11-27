#include <iostream>
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

int main() {
  {
    Rational a(2, 3);
    Rational b(4, 3);
    Rational c = a * b;
    bool equal = c == Rational(8, 9);
    if (!equal) {
      cout << "2/3 * 4/3 != 8/9" << endl;
      return 1;
    }
  }

  {
    Rational a(5, 4);
    Rational b(15, 8);
    Rational c = a / b;
    bool equal = c == Rational(2, 3);
    if (!equal) {
      cout << "5/4 / 15/8 != 2/3" << endl;
      return 2;
    }
  }

  cout << "OK" << endl;
  return 0;
}
