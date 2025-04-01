#include "binary_tree_node.h"
#include "test_framework/generic_test.h"

namespace {

bool is_symmetric(
  const unique_ptr<BinaryTreeNode<int>>& left,
  const unique_ptr<BinaryTreeNode<int>>& right
) {
  if (!left && !right) return true;
  if (!left || !right) return false;

  return left->data == right->data &&
    is_symmetric(left->left, right->right) &&
    is_symmetric(left->right, right->left);
}

bool is_symmetric(const unique_ptr<BinaryTreeNode<int>>& tree) {
  return tree ? is_symmetric(tree->left, tree->right) : true;
}

}

bool IsSymmetric(const unique_ptr<BinaryTreeNode<int>>& tree) {
  return is_symmetric(tree);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree"};
  return GenericTestMain(args, "is_tree_symmetric.cc", "is_tree_symmetric.tsv",
                         &IsSymmetric, DefaultComparator{}, param_names);
}
