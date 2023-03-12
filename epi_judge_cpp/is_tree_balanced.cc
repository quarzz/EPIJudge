#include "binary_tree_node.h"
#include "test_framework/generic_test.h"

#include <iostream>

// #define log(x) std::cout << #x << ": " << (x) << std::endl
#define log(x) nullptr

using node_ptr = unique_ptr<BinaryTreeNode<int>>;

namespace {
  int height(const node_ptr& tree) {
    return tree ? std::max(height(tree->left), height(tree->right)) + 1 : 0;
  }

  // Passing
  bool trivial(const node_ptr& tree) {
    if (!tree) return true;

    const auto left_height = height(tree->left);
    const auto right_height = height(tree->right);
    const auto balanced = std::abs(left_height - right_height) <= 1;

    return balanced ? trivial(tree->left) && trivial(tree->right) : false;
  }

  // The height is valid only if balanced
  struct TraverseData {
    int height;
    bool balanced;
  };

  const TraverseData optimal(const node_ptr& tree) {
    if (!tree) return { 0, true };

    const auto left_data = optimal(tree->left);
    if (!left_data.balanced) return left_data;

    const auto right_data = optimal(tree->right);
    if (!right_data.balanced) return right_data;

    const auto height = std::max(left_data.height, right_data.height) + 1;
    const auto balanced = std::abs(left_data.height - right_data.height) <= 1;
    return { height, balanced };
  }
}

bool IsBalanced(const node_ptr& tree) {
  // return trivial(tree);
  return optimal(tree).balanced;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree"};
  return GenericTestMain(args, "is_tree_balanced.cc", "is_tree_balanced.tsv",
                         &IsBalanced, DefaultComparator{}, param_names);
}
