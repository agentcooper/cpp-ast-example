// Similar to ast_variant_visit.cpp, but using arena (pool) allocation

#include <functional>
#include <iostream>
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
  IdentityNode(const std::string name) : name(std::move(name)) {};
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
  std::vector<std::reference_wrapper<Node>> args;
  CallExpressionNode(std::string callee,
                     std::vector<std::reference_wrapper<Node>> args)
      : callee(std::move(callee)), args(std::move(args)) {}
};

struct PrintVisitor {
  std::string operator()(const NumberNode &e) const {
    return std::to_string(e.value);
  }
  std::string operator()(const IdentityNode &e) const { return e.name; }
  std::string operator()(const BinaryExpressionNode &e) const {
    return "(" + std::visit(*this, e.lhs) + " " + e.op + " " +
           std::visit(*this, e.rhs) + ")";
  }
  std::string operator()(const CallExpressionNode &e) const {
    std::string result = e.callee + "(";
    for (size_t i = 0; i < e.args.size(); ++i) {
      result += std::visit(*this, e.args[i].get());
      if (i < e.args.size() - 1) {
        result += ", ";
      }
    }
    result += ")";
    return result;
  }
};

int main() {
  std::vector<Node> node_storage;
  const size_t max_num_nodes = 100;
  node_storage.reserve(max_num_nodes);

  // (a * (b + f(1, 2)))
  auto &a_node = node_storage.emplace_back(IdentityNode("a"));
  auto &b_node = node_storage.emplace_back(IdentityNode("b"));
  auto &const1 = node_storage.emplace_back(NumberNode(1));
  auto &const2 = node_storage.emplace_back(NumberNode(2));
  std::vector<std::reference_wrapper<Node>> f_args = {const1, const2};
  auto &f_call = node_storage.emplace_back(CallExpressionNode("f", f_args));
  auto &add_expr =
      node_storage.emplace_back(BinaryExpressionNode("+", b_node, f_call));
  auto &root =
      node_storage.emplace_back(BinaryExpressionNode("*", a_node, add_expr));

  PrintVisitor visitor;
  std::cout << std::visit(visitor, root) << std::endl;
}
