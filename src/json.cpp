#include "json.h"
#include <algorithm>
#include <stdexcept>

using namespace std;

namespace Json {

  Document::Document(Node root) : root(move(root)) {
  }

  const Node& Document::GetRoot() const {
    return root;
  }

  Node LoadNode(istream& input);

  Node LoadArray(istream& input) {
    vector<Node> result;

    for (char c; input >> c && c != ']'; ) {
      if (c != ',') {
        input.putback(c);
      }
      result.push_back(LoadNode(input));
    }

    return Node(move(result));
  }

  Node LoadNumber(istream& input) {
    int coeff = 1;
    if (input.peek() == '-') {
      input.get();
      coeff = -1;
    }
    int int_result = 0;
    while (isdigit(input.peek())) {
      int_result *= 10;
      int_result += input.get() - '0';
    }
    if (input.peek() != '.') {
      return Node(coeff * int_result);
    }
    input.get();
    double double_result = int_result;
    for (double denominator = 10; isdigit(input.peek()); denominator *= 10) {
      double_result += (input.get() - '0') / denominator;
    }
    return Node(coeff * double_result);
  }

  Node LoadString(istream& input) {
    string line;
    getline(input, line, '"');
    return Node(move(line));
  }

  Node LoadBool(istream& input) {
    constexpr size_t TRUE_SIZE = 4;
    constexpr size_t FALSE_SIZE = 5;
    string str(TRUE_SIZE, ' ');
    for (size_t i = 0; i < TRUE_SIZE; ++i) {
      input >> str[i];
    }
    if (str == "true") {
      return Node(true);
    }
    str.resize(FALSE_SIZE, ' ');
    for (size_t i = TRUE_SIZE; i < FALSE_SIZE; ++i) {
      input >> str[i];
    }
    if (str == "false") {
      return Node(false);
    }
    throw invalid_argument("Error when parsing bool, string = " + str);
  }

  Node LoadDict(istream& input) {
    map<string, Node> result;

    for (char c; input >> c && c != '}'; ) {
      if (c == ',') {
        input >> c;
      }

      string key = LoadString(input).AsString();
      input >> c;
      result.emplace(move(key), LoadNode(input));
    }

    return Node(move(result));
  }

  Node LoadNode(istream& input) {
    char c;
    input >> c;

    if (c == '[') {
      return LoadArray(input);
    } else if (c == '{') {
      return LoadDict(input);
    } else if (c == '"') {
      return LoadString(input);
    } else if (c == 't' || c == 'f') {
      input.putback(c);
      return LoadBool(input);
    } else {
      input.putback(c);
      return LoadNumber(input);
    }
  }

  Document Load(istream& input) {
    return Document{LoadNode(input)};
  }

  void Node::Save(ostream& output) const {
    visit(NodeVisitor(output), static_cast<NodeType>(*this));
  }

  void Node::NodeVisitor::NewLine() const {
    output << '\n';
    for (size_t i = 0;  i < 2 * offset; ++i) {
      output << ' ';
    }
  }

  void Node::NodeVisitor::operator()(const vector<Node>& node) {
    output << '[';
    if (!node.empty()) {
      ++offset;
      bool first = true;
      for (const auto& item : node) {
        if (!first) {
          output << ',';
        }
        first = false;
        NewLine();
        visit(*this, static_cast<NodeType>(item));
      }
      --offset;
      NewLine();
    }
    output << ']';
  }

  void Node::NodeVisitor::operator()(const map<string, Node>& node) {
    output << '{';
    if (!node.empty()) {
      ++offset;
      bool first = true;
      for (const auto& [key, value] : node) {
        if (!first) {
          output << ',';
        }
        first = false;
        NewLine();
        output << '"' << key << "\": ";
        visit(*this, static_cast<NodeType>(value));
      }
      --offset;
      NewLine();
    }
    output << '}';
  }

  void Node::NodeVisitor::operator()(int node) {
    output << node;
  }

  void Node::NodeVisitor::operator()(const string& node) {
    output << '"' << node << '"';
  }

  void Node::NodeVisitor::operator()(double node) {
    output << node;
  }

  void Node::NodeVisitor::operator()(bool node) {
    output << node;
  }

  void Save(const Document& document, ostream& output) {
    output << boolalpha;
    output.precision(6);
    document.GetRoot().Save(output);
  }
}
