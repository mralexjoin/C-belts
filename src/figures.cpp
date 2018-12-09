#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

constexpr double PI = 3.14;

using namespace std;

class Figure {
public:
  virtual string Name() const = 0;
  virtual double Perimeter() const = 0;
  virtual double Area() const = 0;
private:
  const string name;
};

class Rect : public Figure {
public:
  Rect(const double& width, const double& height) :
    width(width),
    height(height) {}
  string Name() const override {
    return "RECT";
  }
  double Perimeter() const override {
    return 2 * width + 2 * height;
  }
  double Area() const override {
    return width * height;
  }
private:
  const double width;
  const double height;
};

class Triangle : public Figure {
public:
  Triangle(const double& a, const double& b, const double& c) :
    a(a),
    b(b),
    c(c) {}
  string Name() const override {
    return "TRIANGLE";
  }
  double Perimeter() const override {
    return a + b + c;
  }
  double Area() const override {
    const double p = Perimeter() / 2;
    return sqrt(p * (p - a) * (p - b) * (p - c));
  }
private:
  const double a;
  const double b;
  const double c;
};

class Circle : public Figure {
public:
  Circle(const double& r) :
    r(r) {}
  string Name() const override {
    return "CIRCLE";
  }
  double Perimeter() const override {
    return 2 * PI * r;
  }
  double Area() const override {
    return PI * r * r;
  }
private:
  const double r;
};

shared_ptr<Figure> CreateFigure(istream& stream) {
  string name;
  stream >> name;
  if (name == "RECT") {
    double width, height;
    stream >> width >> height;
    return make_shared<Rect>(width, height);
  }
  else if (name == "TRIANGLE") {
    double a, b, c;
    stream >> a >> b >> c;
    return make_shared<Triangle>(a, b, c);
  }
  else {
    double r;
    stream >> r;
    return make_shared<Circle>(r);
  }
}

int main() {
  vector<shared_ptr<Figure>> figures;
  for (string line; getline(cin, line); ) {
    istringstream is(line);

    string command;
    is >> command;
    if (command == "ADD") {
      figures.push_back(CreateFigure(is));
    } else if (command == "PRINT") {
      for (const auto& current_figure : figures) {
        cout << fixed << setprecision(3)
             << current_figure->Name() << " "
             << current_figure->Perimeter() << " "
             << current_figure->Area() << endl;
      }
    }
  }
  return 0;
}
