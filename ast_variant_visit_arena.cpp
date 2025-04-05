// Similar to ast_variant_visit.cpp, but using arena (pool) allocation

#include <functional>
#include <iostream>
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
  IdentifierNode(const std::string name) : name(std::move(name)) {};
};

struct NumberNode {
  int value;
  NumberNode(const int value) : value(value) {};
};

struct BinaryExpressionNode {
  std::string op;
  Node &lhs;
  Node &rhs;
  BinaryExpressionNode(std::string op, Node &lhs, Node &rhs)
      : op(std::move(op)), lhs(lhs), rhs(rhs) {}
};

struct CallExpressionNode {
  std::string callee;
  std::vector<std::reference_wrapper<Node>> arguments;
  CallExpressionNode(std::string callee,
                     std::vector<std::reference_wrapper<Node>> arguments)
      : callee(std::move(callee)), arguments(std::move(arguments)) {}
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
  std::string visit(const Node &node) const { return std::visit(*this, node); }
};

int main() {
  std::vector<Node> node_storage;
  const size_t max_num_nodes = 100;
  node_storage.reserve(max_num_nodes);

  // (a * (b + f(1, 2)))
  auto &a_node = node_storage.emplace_back(IdentifierNode("a"));
  auto &b_node = node_storage.emplace_back(IdentifierNode("b"));
  auto &const1 = node_storage.emplace_back(NumberNode(1));
  auto &const2 = node_storage.emplace_back(NumberNode(2));
  std::vector<std::reference_wrapper<Node>> f_arguments = {const1, const2};
  auto &f_call =
      node_storage.emplace_back(CallExpressionNode("f", f_arguments));
  auto &add_expr =
      node_storage.emplace_back(BinaryExpressionNode("+", b_node, f_call));
  auto &root =
      node_storage.emplace_back(BinaryExpressionNode("*", a_node, add_expr));

  PrintVisitor visitor;
  std::cout << std::visit(visitor, root) << std::endl;
}
