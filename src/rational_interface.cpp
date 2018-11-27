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

int main() {
  {
    const Rational r(3, 10);
    if (r.Numerator() != 3 || r.Denominator() != 10) {
      cout << "Rational(3, 10) != 3/10" << endl;
      return 1;
    }
  }

  {
    const Rational r(8, 12);
    if (r.Numerator() != 2 || r.Denominator() != 3) {
      cout << "Rational(8, 12) != 2/3" << endl;
      return 2;
    }
  }

  {
    const Rational r(-4, 6);
    if (r.Numerator() != -2 || r.Denominator() != 3) {
      cout << "Rational(-4, 6) != -2/3, but " << r.Numerator() << '/' << r.Denominator() << endl;
      return 3;
    }
  }

  {
    const Rational r(4, -6);
    if (r.Numerator() != -2 || r.Denominator() != 3) {
      cout << "Rational(4, -6) != -2/3" << endl;
      return 3;
    }
  }

  {
    const Rational r(0, 15);
    if (r.Numerator() != 0 || r.Denominator() != 1) {
      cout << "Rational(0, 15) != 0/1" << endl;
      return 4;
    }
  }

  {
    const Rational defaultConstructed;
    if (defaultConstructed.Numerator() != 0 || defaultConstructed.Denominator() != 1) {
      cout << "Rational() != 0/1" << endl;
      return 5;
    }
  }

  cout << "OK" << endl;
  return 0;
}
