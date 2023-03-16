#include <memory>

#include "binary_tree_node.h"
#include "test_framework/generic_test.h"
using std::unique_ptr;

#define log(x) std::cout << #x << ": " << (x) << std::endl

namespace {
  // max is valid only if is_bst
  struct TraverseData {
    int max;
    int min;
    bool is_bst;
  };

  // Passing
  const TraverseData post_order_minmax(const unique_ptr<BinaryTreeNode<int>>& tree) {
    if (!tree) return {
      std::numeric_limits<int>::min(),
      std::numeric_limits<int>::max(),
      true
    };

    const auto left_data = post_order_minmax(tree->left);
    if (!left_data.is_bst) return left_data;

    const auto right_data = post_order_minmax(tree->right);
    if (!right_data.is_bst) return right_data;

    return {
      std::max(tree->data, right_data.max),
      std::min(tree->data, left_data.min),
      tree->data >= left_data.max && tree->data <= right_data.min
    };
  }

  bool in_range(int val, int from, int to) {
    return val >= from && val <= to;
  }

  // Passing
  // This is actually the same approach, but with a bit smarter borde checking
  //   and more efficient as well.
  bool pre_order_ranged(
    const unique_ptr<BinaryTreeNode<int>>& tree,
    int from = std::numeric_limits<int>::min(),
    int to = std::numeric_limits<int>::max()
  ) {
    if (!tree) return true;

    return in_range(tree->data, from, to)
      && pre_order_ranged(tree->left, from, tree->data)
      && pre_order_ranged(tree->right, tree->data, to);
  }

  int prev = std::numeric_limits<int>::min();

  // Passing
  bool in_order_checking_prev(const unique_ptr<BinaryTreeNode<int>>& tree) {
    if (!tree) return true;

    const auto left_res = in_order_checking_prev(tree->left);
    if (!left_res) return false;

    if (tree->data < prev) return false;

    prev = tree->data;
    return in_order_checking_prev(tree->right);
  }

  struct QueueEntry {
    BinaryTreeNode<int>* node;
    int from;
    int to;
  };

  // Passing, but is the slowest
  bool bfs_ranged(const unique_ptr<BinaryTreeNode<int>>& tree) {
    if (!tree) return true;

    std::queue<QueueEntry> queue;
    queue.push({
      tree.get(),
      std::numeric_limits<int>::min(),
      std::numeric_limits<int>::max()
    });

    while (!queue.empty()) {
      const auto entry = queue.front();
      queue.pop();

      if (!in_range(entry.node->data, entry.from, entry.to))
        return false;

      if (entry.node->left)
        queue.push({ entry.node->left.get(), entry.from, entry.node->data });
      if (entry.node->right)
        queue.push({ entry.node->right.get(), entry.node->data, entry.to });
    }

    return true;
  }
}

bool IsBinaryTreeBST(const unique_ptr<BinaryTreeNode<int>>& tree) {
  return bfs_ranged(tree);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree"};
  return GenericTestMain(args, "is_tree_a_bst.cc", "is_tree_a_bst.tsv",
                         &IsBinaryTreeBST, DefaultComparator{}, param_names);
}
