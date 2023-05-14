#include <memory>
#include <vector>

#include "bst_node.h"
#include "test_framework/generic_test.h"
using std::unique_ptr;
using std::vector;

namespace {
  void reverse_in_order(
    const unique_ptr<BstNode<int>>& tree,
    int k,
    vector<int>& largest_elements
  ) {
    if (!tree || largest_elements.size() == k) return;

    reverse_in_order(tree->right, k, largest_elements);

    if (largest_elements.size() == k) return;
    largest_elements.emplace_back(tree->data);

    reverse_in_order(tree->left, k, largest_elements);
  }
}

vector<int> FindKLargestInBST(const unique_ptr<BstNode<int>>& tree, int k) {
  // TODO - you fill in here.
  vector<int> largest_elements;
  reverse_in_order(tree, k, largest_elements);

  return largest_elements;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree", "k"};
  return GenericTestMain(args, "k_largest_values_in_bst.cc",
                         "k_largest_values_in_bst.tsv", &FindKLargestInBST,
                         UnorderedComparator{}, param_names);
}
