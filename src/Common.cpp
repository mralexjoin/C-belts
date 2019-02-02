#include "Common.h"

#include <sstream>

using namespace std;

namespace Expressions {

class Value : public Expression {
public:
  Value(int value) : value(value) {}
  int Evaluate() const override {
    return value;
  }
  string ToString() const override {
    return to_string(value);
  }
private:
  int value;
};

class Binary : public Expression {
public:
  Binary(ExpressionPtr lhs, ExpressionPtr rhs) :
    lhs(move(lhs)),
    rhs(move(rhs)) {}
  int Evaluate() const final {
    return EvaluateBinaryOp(lhs->Evaluate(), rhs->Evaluate());
  }
  string ToString() const final {
    ostringstream output;
    output << '(' << lhs->ToString() << ')'
           << GetOpSymbol() 
           << '(' << rhs->ToString() << ')';
    return output.str();
  }
private:
  virtual int EvaluateBinaryOp(int lhs, int rhs) const = 0;
  virtual char GetOpSymbol() const = 0;
  ExpressionPtr lhs, rhs;
};

class Sum : public Binary {
public:
  Sum(ExpressionPtr lhs, ExpressionPtr rhs) :
    Binary(move(lhs), move(rhs)) {}
private:
  int EvaluateBinaryOp(int lhs, int rhs) const override {
    return lhs + rhs;
  }
  char GetOpSymbol() const override {
    return '+';
  }
};

class Product : public Binary {
public:
  Product(ExpressionPtr lhs, ExpressionPtr rhs) :
    Binary(move(lhs), move(rhs)) {}
private:
  int EvaluateBinaryOp(int lhs, int rhs) const override {
    return lhs * rhs;
  }
  char GetOpSymbol() const override {
    return '*';
  }
};

}

ExpressionPtr Value(int value) {
  return make_unique<Expressions::Value>(value);
}
ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
  return make_unique<Expressions::Sum>(move(left), move(right));
}
ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
  return make_unique<Expressions::Product>(move(left), move(right));
}
