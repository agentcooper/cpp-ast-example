// Example of Abstract Syntax Tree using std::variant and std::visit

#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct IdentifierNode;
struct NumberNode;
struct BinaryExpressionNode;
struct CallExpressionNode;

using Node = std::variant<IdentifierNode, NumberNode, BinaryExpressionNode,
                          CallExpressionNode>;

struct IdentifierNode {
  std::string name;
  IdentifierNode(const std::string name) : name(name) {};
};

struct NumberNode {
  int value;
  NumberNode(const int value) : value(value) {};
};

struct BinaryExpressionNode {
  std::string op;
  std::unique_ptr<Node> lhs;
  std::unique_ptr<Node> rhs;
  BinaryExpressionNode(const std::string op, std::unique_ptr<Node> lhs,
                       std::unique_ptr<Node> rhs)
      : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {};
};

struct CallExpressionNode {
  std::string callee;
  std::vector<std::unique_ptr<Node>> arguments;
  CallExpressionNode(const std::string callee,
                     std::vector<std::unique_ptr<Node>> arguments)
      : callee(callee), arguments(std::move(arguments)) {};
};

struct PrintVisitor {
  std::string operator()(const NumberNode &node) const {
    return std::to_string(node.value);
  }
  std::string operator()(const IdentifierNode &node) const { return node.name; }
  std::string operator()(const BinaryExpressionNode &node) const {
    return "(" + visit(node.lhs) + " " + node.op + " " + visit(node.rhs) + ")";
  }
  std::string operator()(const CallExpressionNode &node) const {
    std::string result = node.callee + "(";
    for (size_t i = 0; i < node.arguments.size(); ++i) {
      result += visit(node.arguments[i]);
      if (i < node.arguments.size() - 1) {
        result += ", ";
      }
    }
    result += ")";
    return result;
  }
  std::string visit(const std::unique_ptr<Node> &node) const {
    return std::visit(*this, *node);
  }
};

std::unique_ptr<Node> make(Node &&node) {
  return std::make_unique<Node>(std::move(node));
}

int main() {
  // (a * (b + f(1, 2)))
  std::vector<std::unique_ptr<Node>> args;
  args.push_back(make(NumberNode(1)));
  args.push_back(make(NumberNode(2)));
  auto functionCall = make(CallExpressionNode("f", std::move(args)));
  auto root = make(BinaryExpressionNode(
      "*", make(IdentifierNode("a")),
      make(BinaryExpressionNode("+", make(IdentifierNode("b")),
                                std::move(functionCall)))));

  PrintVisitor visitor{};
  std::cout << visitor.visit(root) << std::endl;
}
