// Example of Abstract Syntax Tree using inheritance and visitor pattern

#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct IdentityNode;
struct NumberNode;
struct BinaryExpressionNode;
struct CallExpressionNode;

struct Visitor {
  virtual void visit(IdentityNode &node) = 0;
  virtual void visit(NumberNode &node) = 0;
  virtual void visit(BinaryExpressionNode &node) = 0;
  virtual void visit(CallExpressionNode &node) = 0;
  virtual ~Visitor() = default;
};

struct Node {
  virtual void accept(Visitor &v) = 0;
  virtual ~Node() = default;
};

struct IdentityNode : public Node {
  std::string name;
  IdentityNode(const std::string name) : name(name){};
  void accept(Visitor &v) override { v.visit(*this); }
};

struct NumberNode : public Node {
  int value;
  NumberNode(const int value) : value(value){};
  void accept(Visitor &v) override { v.visit(*this); }
};

struct BinaryExpressionNode : public Node {
  std::string name;
  std::unique_ptr<Node> lhs;
  std::unique_ptr<Node> rhs;
  BinaryExpressionNode(const std::string name, std::unique_ptr<Node> lhs,
                       std::unique_ptr<Node> rhs)
      : name(name), lhs(std::move(lhs)), rhs(std::move(rhs)){};
  void accept(Visitor &v) override { v.visit(*this); }
};

struct CallExpressionNode : public Node {
  std::string callee;
  std::vector<std::unique_ptr<Node>> arguments;
  CallExpressionNode(const std::string callee,
                     std::vector<std::unique_ptr<Node>> arguments)
      : callee(callee), arguments(std::move(arguments)){};
  void accept(Visitor &v) override { v.visit(*this); }
};

struct PrintVisitor : public Visitor {
  std::string value;
  std::string visit(std::unique_ptr<Node> &node) {
    node->accept(*this);
    return value;
  }
  void visit(IdentityNode &node) { value = node.name; }
  void visit(NumberNode &node) { value = std::to_string(node.value); }
  void visit(BinaryExpressionNode &node) {
    value =
        "(" + visit(node.lhs) + " " + node.name + " " + visit(node.rhs) + ")";
  }
  void visit(CallExpressionNode &node) {
    std::string s;
    for (auto &argument : node.arguments) {
      s += visit(argument);
      if (argument != node.arguments.back()) {
        s += ", ";
      }
    }
    value = node.callee + "(" + s + ")";
  }
};

int main() {
  // (a * (b + f(1, 2)))
  std::vector<std::unique_ptr<Node>> args;
  args.push_back(std::make_unique<NumberNode>(1));
  args.push_back(std::make_unique<NumberNode>(2));
  auto functionCall =
      std::make_unique<CallExpressionNode>("f", std::move(args));
  auto root = std::make_unique<BinaryExpressionNode>(
      "*", std::make_unique<IdentityNode>("a"),

      std::make_unique<BinaryExpressionNode>(
          "+", std::make_unique<IdentityNode>("b"), std::move(functionCall)));

  PrintVisitor visitor{};
  root->accept(visitor);
  std::cout << visitor.value << std::endl;
}