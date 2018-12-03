#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

template <class T>
ostream& operator << (ostream& os, const vector<T>& s) {
  os << "{";
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << x;
  }
  return os << "}";
}

template <class T>
ostream& operator << (ostream& os, const set<T>& s) {
  os << "{";
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << x;
  }
  return os << "}";
}

template <class K, class V>
ostream& operator << (ostream& os, const map<K, V>& m) {
  os << "{";
  bool first = true;
  for (const auto& kv : m) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << kv.first << ": " << kv.second;
  }
  return os << "}";
}

template<class T, class U>
void AssertEqual(const T& t, const U& u, const string& hint = {}) {
  if (t != u) {
    ostringstream os;
    os << "Assertion failed: " << t << " != " << u;
    if (!hint.empty()) {
       os << " hint: " << hint;
    }
    throw runtime_error(os.str());
  }
}

void Assert(bool b, const string& hint) {
  AssertEqual(b, true, hint);
}

class TestRunner {
public:
  template <class TestFunc>
  void RunTest(TestFunc func, const string& test_name) {
    try {
      func();
      cerr << test_name << " OK" << endl;
    } catch (exception& e) {
      ++fail_count;
      cerr << test_name << " fail: " << e.what() << endl;
    } catch (...) {
      ++fail_count;
      cerr << "Unknown exception caught" << endl;
    }
  }

  ~TestRunner() {
    if (fail_count > 0) {
      cerr << fail_count << " unit tests failed. Terminate" << endl;
      exit(1);
    }
  }

private:
  int fail_count = 0;
};

// class Rational {
// public:
//   Rational() {
//     // Реализуйте конструктор по умолчанию
//     numerator = 0;
//     denominator = 1;
//   }

//   Rational(int new_numerator, int new_denominator) {
//     // Реализуйте конструктор
//     if (new_denominator == 0)
//       throw invalid_argument("Invalid argument");

//     bool negative = (new_numerator < 0 && new_denominator > 0
//                      || new_numerator > 0 && new_denominator < 0);
//     int positive_numerator = abs(new_numerator);
//     int positive_denominator = abs(new_denominator);
//     int s = scf(positive_numerator, positive_denominator);
//     numerator = positive_numerator / s;
//     denominator = positive_denominator / s;
//     if (negative)
//       numerator = -numerator;
//   }

//   int Numerator() const {
//     // Реализуйте этот метод
//     return numerator;
//   }

//   int Denominator() const {
//     // Реализуйте этот метод
//     return denominator;
//   }

// private:
//   // Добавьте поля
//   int numerator;
//   int denominator;
//   int scf(int a, int b) const {
//     while (a > 0 && b > 0) {
//       if (a > b)
//         a %= b;
//       else
//         b %= a;
//     }
//     if (a != 0)
//       return a;
//     return b;
//   }
// };

void TestDefaultConstructor() {
  Rational r;
  AssertEqual(r.Numerator(), 0, "Default constructor: numerator != 0");
  AssertEqual(r.Denominator(), 1, "Default constructor: denominator != 1");
}

void TestShrink() {
  Rational r(16, 32);
  AssertEqual(r.Numerator(), 1, "Shrinking: numerator != 1");
  AssertEqual(r.Denominator(), 2, "Shrinking: denominator != 2");
}

void TestNegative() {
  {
    Rational r(-1, 2);
    AssertEqual(r.Numerator(), -1, "Negative numerator: numerator != -1");
    AssertEqual(r.Denominator(), 2, "Negative numerator: denominator != 2");
  }
  {
    Rational r(1, -2);
    AssertEqual(r.Numerator(), -1, "Negative denominator: numerator != -1");
    AssertEqual(r.Denominator(), 2, "Negative denominator: denominator != 2");
  }
}

void TestPositive() {
  {
    Rational r(1, 2);
    AssertEqual(r.Numerator(), 1, "Positive numerator/denominator: numerator != 1");
    AssertEqual(r.Denominator(), 2, "Positive numerator/denominator: denominator != 2");
  }
  {
    Rational r(-1, -2);
    AssertEqual(r.Numerator(), 1, "Negative numerator/denominator: numerator != 1");
    AssertEqual(r.Denominator(), 2, "Negative numerator/denominator: denominator != 2");
  }
}

void TestZero() {
  Rational r(0, 2);
  AssertEqual(r.Denominator(), 1, "Zero: denominator != 1");
}

int main() {
  TestRunner runner;
  runner.RunTest(TestDefaultConstructor, "TestDefaultConstructor");
  runner.RunTest(TestShrink, "TestShrink");
  runner.RunTest(TestNegative, "TestNegative");
  runner.RunTest(TestPositive, "TestPositive");
  runner.RunTest(TestZero, "TestZero");
  // добавьте сюда свои тесты
  return 0;
}
