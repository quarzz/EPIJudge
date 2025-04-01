#include <memory>

#include "binary_tree_node.h"
#include "test_framework/binary_tree_utils.h"
#include "test_framework/generic_test.h"
#include "test_framework/test_failure.h"
#include "test_framework/timed_executor.h"

namespace {

unsigned int NONE = 0;
unsigned int FIRST = 1;
unsigned int SECOND = 2;
unsigned int ALL = 3;

struct TraverseData {
  BinaryTreeNode<int>* result;
  unsigned int found_nodes;
};

TraverseData lca(
  const unique_ptr<BinaryTreeNode<int>>& tree,
  const unique_ptr<BinaryTreeNode<int>>& first,
  const unique_ptr<BinaryTreeNode<int>>& second
) {
  if (!tree) return {nullptr, NONE};

  const auto found_nodes = FIRST * (tree == first) + SECOND * (tree == second);

  const auto left_result = lca(tree->left, first, second);
  if (left_result.found_nodes == ALL) return left_result;
  if (left_result.found_nodes + found_nodes == ALL) return {tree.get(), ALL};

  const auto right_result = lca(tree->right, first, second);
  if (right_result.found_nodes == ALL) return right_result;
  if (right_result.found_nodes + left_result.found_nodes + found_nodes == ALL) return {tree.get(), ALL};

  return {nullptr, right_result.found_nodes + left_result.found_nodes + found_nodes};
}

}

using std::unique_ptr;
BinaryTreeNode<int>* Lca(const unique_ptr<BinaryTreeNode<int>>& tree,
                         const unique_ptr<BinaryTreeNode<int>>& node0,
                         const unique_ptr<BinaryTreeNode<int>>& node1) {
  return lca(tree, node0, node1).result;
}

int LcaWrapper(TimedExecutor& executor,
               const unique_ptr<BinaryTreeNode<int>>& tree, int key0,
               int key1) {
  const unique_ptr<BinaryTreeNode<int>>& node0 = MustFindNode(tree, key0);
  const unique_ptr<BinaryTreeNode<int>>& node1 = MustFindNode(tree, key1);

  auto result = executor.Run([&] { return Lca(tree, node0, node1); });

  if (!result) {
    throw TestFailure("Result can not be nullptr");
  }
  return result->data;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"executor", "tree", "key0", "key1"};
  return GenericTestMain(args, "lowest_common_ancestor.cc",
                         "lowest_common_ancestor.tsv", &LcaWrapper,
                         DefaultComparator{}, param_names);
}
