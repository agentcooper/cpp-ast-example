// Example of Abstract Syntax Tree using inheritance and visitor pattern

#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct IdentifierNode;
struct NumberNode;
struct BinaryExpressionNode;
struct CallExpressionNode;

struct Visitor {
  virtual void visit(IdentifierNode &node) = 0;
  virtual void visit(NumberNode &node) = 0;
  virtual void visit(BinaryExpressionNode &node) = 0;
  virtual void visit(CallExpressionNode &node) = 0;
  virtual ~Visitor() = default;
};

struct Node {
  virtual void accept(Visitor &v) = 0;
  virtual ~Node() = default;
};

struct IdentifierNode : public Node {
  std::string name;
  IdentifierNode(const std::string name) : name(name) {};
  void accept(Visitor &v) override { v.visit(*this); }
};

struct NumberNode : public Node {
  int value;
  NumberNode(const int value) : value(value) {};
  void accept(Visitor &v) override { v.visit(*this); }
};

struct BinaryExpressionNode : public Node {
  std::string op;
  std::unique_ptr<Node> lhs;
  std::unique_ptr<Node> rhs;
  BinaryExpressionNode(const std::string op, std::unique_ptr<Node> lhs,
                       std::unique_ptr<Node> rhs)
      : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {};
  void accept(Visitor &v) override { v.visit(*this); }
};

struct CallExpressionNode : public Node {
  std::string callee;
  std::vector<std::unique_ptr<Node>> arguments;
  CallExpressionNode(const std::string callee,
                     std::vector<std::unique_ptr<Node>> arguments)
      : callee(callee), arguments(std::move(arguments)) {};
  void accept(Visitor &v) override { v.visit(*this); }
};

struct PrintVisitor : public Visitor {
  std::string value;
  std::string visit(std::unique_ptr<Node> &node) {
    node->accept(*this);
    return value;
  }
  void visit(IdentifierNode &node) { value = node.name; }
  void visit(NumberNode &node) { value = std::to_string(node.value); }
  void visit(BinaryExpressionNode &node) {
    value = "(" + visit(node.lhs) + " " + node.op + " " + visit(node.rhs) + ")";
  }
  void visit(CallExpressionNode &node) {
    std::string result = node.callee + "(";
    for (size_t i = 0; i < node.arguments.size(); ++i) {
      result += visit(node.arguments[i]);
      if (i < node.arguments.size() - 1) {
        result += ", ";
      }
    }
    value = result;
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
      "*", std::make_unique<IdentifierNode>("a"),

      std::make_unique<BinaryExpressionNode>(
          "+", std::make_unique<IdentifierNode>("b"), std::move(functionCall)));

  PrintVisitor visitor{};
  root->accept(visitor);
  std::cout << visitor.value << std::endl;
}
