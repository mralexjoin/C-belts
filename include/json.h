#pragma once

#include <istream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace Json {

  class Node;
  using NodeType = std::variant<std::vector<Node>,
                            std::map<std::string, Node>,
                            int,
                            std::string,
                            double,
                            bool>;
  class Node : NodeType {
  private:
    struct NodeVisitor {
      NodeVisitor(std::ostream& output) : output(output) {}
      void operator()(const std::vector<Node>& node);
      void operator()(const std::map<std::string, Node>& node);
      void operator()(int node);
      void operator()(const std::string& node);
      void operator()(double node);
      void operator()(bool node);
    private:
      void NewLine() const;
      std::ostream& output;
      size_t offset = 0;
    };
  public:
    using variant::variant;
    auto& AsArray() {
      return std::get<std::vector<Node>>(*this);
    }
    auto& AsMap() {
      return std::get<std::map<std::string, Node>>(*this);
    }
    int& AsInt() {
      return std::get<int>(*this);
    }
    auto& AsString() {
      return std::get<std::string>(*this);
    }
    double& AsDouble() {
      return std::get<double>(*this);
    }
    bool& AsBool() {
      return std::get<bool>(*this);
    }
    const auto& AsArray() const {
      return std::get<std::vector<Node>>(*this);
    }
    const auto& AsMap() const {
      return std::get<std::map<std::string, Node>>(*this);
    }
    int AsInt() const {
      return std::get<int>(*this);
    }
    const auto& AsString() const {
      return std::get<std::string>(*this);
    }
    double AsDouble() const {
      return std::holds_alternative<double>(*this) ? std::get<double>(*this) : std::get<int>(*this);
    }
    bool AsBool() const {
      return std::get<bool>(*this);
    }
    void Save(std::ostream& output) const;
  };

  class Document {
  public:
    explicit Document(Node root);

    const Node& GetRoot() const;

  private:
    Node root;
  };

  Document Load(std::istream& input);
  void Save(const Document& document, std::ostream& output);
}
