// Example of Abstract Syntax Tree using std::variant and std::visit

#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct IdentityNode;
struct NumberNode;
struct BinaryExpressionNode;
struct CallExpressionNode;

using Node = std::variant<IdentityNode, NumberNode, BinaryExpressionNode,
                          CallExpressionNode>;

struct IdentityNode {
  std::string name;
  IdentityNode(const std::string name) : name(name){};
};

struct NumberNode {
  int value;
  NumberNode(const int value) : value(value){};
};

struct BinaryExpressionNode {
  std::string name;
  std::unique_ptr<Node> lhs;
  std::unique_ptr<Node> rhs;
  BinaryExpressionNode(const std::string name, std::unique_ptr<Node> lhs,
                       std::unique_ptr<Node> rhs)
      : name(name), lhs(std::move(lhs)), rhs(std::move(rhs)){};
};

struct CallExpressionNode {
  std::string callee;
  std::vector<std::unique_ptr<Node>> arguments;
  CallExpressionNode(const std::string callee,
                     std::vector<std::unique_ptr<Node>> arguments)
      : callee(callee), arguments(std::move(arguments)){};
};

struct Visitor {
  std::string operator()(IdentityNode &node) { return node.name; }
  std::string operator()(NumberNode &node) {
    return std::to_string(node.value);
  }
  std::string operator()(BinaryExpressionNode &node) {
    return "(" + visit(node.lhs) + " " + node.name + " " + visit(node.rhs) +
           ")";
  }
  std::string operator()(CallExpressionNode &node) {
    std::string s;
    for (auto &argument : node.arguments) {
      s += visit(argument);
      if (argument != node.arguments.back()) {
        s += ", ";
      }
    }
    return node.callee + "(" + s + ")";
  }
  std::string visit(std::unique_ptr<Node> &node) {
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
      "*", make(IdentityNode("a")),
      make(BinaryExpressionNode("+", make(IdentityNode("b")),
                                std::move(functionCall)))));

  Visitor visitor{};
  std::cout << visitor.visit(root) << std::endl;
}