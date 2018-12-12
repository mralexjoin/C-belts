#pragma once

#include <memory>
#include <string>
#include "date.h"

enum class Comparison {
  Less,
  LessOrEqual,
  Greater,
  GreaterOrEqual,
  Equal,
  NotEqual
};

enum class LogicalOperation {
  And,
  Or
};

class Node {
 public:
  virtual bool Evaluate(const Date& date, const std::string& event) const = 0;
 protected:
  template<typename T> bool Compare(const Comparison& cmp, const T& lhs, const T& rhs) const {
    switch (cmp) {
    case Comparison::Less:
      return lhs < rhs;
    case Comparison::LessOrEqual:
      return lhs <= rhs;
    case Comparison::Greater:
      return lhs > rhs;
    case Comparison::GreaterOrEqual:
      return lhs >= rhs;
    case Comparison::Equal:
      return lhs == rhs;
    case Comparison::NotEqual:
      return lhs != rhs;
    }
    return false;
  }
};

class EmptyNode : public Node {
 public:
  inline bool Evaluate(const Date& date, const std::string& event) const override {
    return true;
  }
};

class DateComparisonNode : public Node {
 public:
  DateComparisonNode(const Comparison& cmp, const Date& date) :
    cmp_(cmp),
    date_(date) {}
  inline bool Evaluate(const Date& date, const std::string& event) const override {
    return Compare(cmp_, date, date_);
  }
 private:
  const Comparison cmp_;
  const Date date_;
};

class EventComparisonNode : public Node {
 public:
  EventComparisonNode(const Comparison& cmp, const std::string& event) :
    cmp_(cmp),
    event_(event) {}
  inline bool Evaluate(const Date& date, const std::string& event) const override {
    return Compare(cmp_, event, event_);
  }
 private:
  const Comparison cmp_;
  const std::string event_;
};

class LogicalOperationNode : public Node {
 public:
  LogicalOperationNode(const LogicalOperation& logical_operation,
                       const std::shared_ptr<Node> lhs,
                       const std::shared_ptr<Node> rhs):
    logical_operation(logical_operation),
    lhs(lhs),
    rhs(rhs) {}
  bool Evaluate(const Date& date, const std::string& event) const override;
 private:
  const LogicalOperation logical_operation;
  const std::shared_ptr<Node> lhs;
  const std::shared_ptr<Node> rhs;
};
