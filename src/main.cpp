#include <deque>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

class ExpressionBuilder {
public:
  ExpressionBuilder(int x) {
    expression.push_back(std::to_string(x));
  }
  void AddOperation(const std::string& operation) {
    std::istringstream stream(operation);
    char c;
    int x;
    stream >> c >> x;

    int priority = GetPriority(c);
    if (priority > last_priority) {
      AddFrontBracket();
      AddBackBracket();
    }
    last_priority = priority;
    AddBackSpace();
    expression.push_back(std::string(1, c));
    AddBackSpace();
    expression.push_back(std::to_string(x));
  }
  std::string Build() const {
    std::ostringstream stream;
    copy(expression.begin(),
         expression.end(),
         std::ostream_iterator<std::string>(stream, ""));
    return stream.str();
  }
private:
  std::deque<std::string> expression;
  int last_priority = 2;
  void AddFrontBracket() {
    expression.push_front("(");
  }
  void AddBackBracket() {
    expression.push_back(")");
  }
  void AddBackSpace() {
    expression.push_back(" ");
  }
  int GetPriority(char operation) {
    switch(operation) {
    case '+': case '-':
      return 0;
    case '*': case '/':
      return 1;
    default:
      return 2;
    }
  }
};

int main() {
  std::string line;
  getline(std::cin, line);
  std::istringstream stream(line);
  int x;
  stream >> x;

  getline(std::cin, line);
  stream = std::istringstream(line);
  size_t n;
  stream >> n;

  ExpressionBuilder builder(x);
  for (size_t i = 0; i < n; i++) {

    getline(std::cin, line);
    builder.AddOperation(line);
  }

  std::cout << builder.Build();

  return 0;
}
