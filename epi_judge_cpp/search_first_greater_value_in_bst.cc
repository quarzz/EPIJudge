#include <memory>

#include "bst_node.h"
#include "test_framework/generic_test.h"
using std::unique_ptr;

namespace {
  using Node = BstNode<int>;

  Node* impl(const unique_ptr<BstNode<int>>& tree, int k) {
    auto current_node = tree.get();
    Node* last_greater_node = nullptr;

    while (current_node) {
      if (current_node->data <= k) {
        current_node = current_node->right.get();
      } else {
        last_greater_node = current_node;
        current_node = current_node->left.get();
      }
    }

    return last_greater_node;
  }

  Node* variant(const unique_ptr<BstNode<int>>& tree, int val) {
    auto cur = tree.get();
    Node* last_equal = nullptr;

    while (cur) {
      if (cur->data == val)
        last_equal = cur;

      cur = cur->data < val ? cur->right.get() : cur->left.get();
    }

    return last_equal;
  }
}

BstNode<int>* FindFirstGreaterThanK(
  const unique_ptr<BstNode<int>>& tree,
  int k
) {
  // TODO - you fill in here.
  return impl(tree, k);
}
int FindFirstGreaterThanKWrapper(const unique_ptr<BstNode<int>>& tree, int k) {
  auto result = FindFirstGreaterThanK(tree, k);
  return result ? result->data : -1;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree", "k"};
  return GenericTestMain(args, "search_first_greater_value_in_bst.cc",
                         "search_first_greater_value_in_bst.tsv",
                         &FindFirstGreaterThanKWrapper, DefaultComparator{},
                         param_names);
}
