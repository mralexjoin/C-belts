#include "node.h"

bool LogicalOperationNode::Evaluate(const Date& date, const std::string& event) const {
  bool left = lhs->Evaluate(date, event);
  bool right = rhs->Evaluate(date, event);
  switch(logical_operation) {
  case LogicalOperation::And:
    return left && right;
  case LogicalOperation::Or:
    return left || right;
  }
  return true;
}
