#include <functional>
#include <memory>
#include <queue>
#include <vector>

#include "binary_tree_node.h"
#include "test_framework/generic_test.h"
using std::unique_ptr;
using std::vector;

namespace {
  using node_ptr = std::unique_ptr<BinaryTreeNode<int>>;
  using node_ptr_ref = std::reference_wrapper<const node_ptr>;

  struct NodeWrapperWithLevel {
    node_ptr_ref node;
    int level;
  };

  // Passing
  auto using_reference_wrappers(const node_ptr& tree) {
    vector<vector<int>> levels;
    std::queue<NodeWrapperWithLevel> q;
    q.push({ std::ref(tree), 1 });

    while (!q.empty()) {
      const auto [node_ref, level] = q.front(); q.pop();
      const auto& node = node_ref.get();

      if (!node) continue;

      if (levels.size() != level) levels.emplace_back();

      levels[level - 1].emplace_back(node->data);
      q.push({ std::ref(node->left), level + 1 });
      q.push({ std::ref(node->right), level + 1 });
    }

    return levels;
  }

  struct NodePtrWithLevel {
    BinaryTreeNode<int>* node;
    int level;
  };

  // Passing
  auto using_raw_pointers(const node_ptr& tree) {
    vector<vector<int>> levels;
    std::queue<NodePtrWithLevel> q;
    q.push({ tree.get(), 1 });

    while (!q.empty()) {
      const auto [node, level] = q.front(); q.pop();

      if (!node) continue;

      if (levels.size() != level) levels.emplace_back();

      levels[level - 1].emplace_back(node->data);
      q.push({ node->left.get(), level + 1 });
      q.push({ node->right.get(), level + 1 });
    }

    return levels;
  }

  // Passing
  auto using_raw_pointers_and_two_queues(const node_ptr& tree) {
    std::vector<std::vector<int>> levels;
    if (!tree.get()) return levels;

    std::queue<BinaryTreeNode<int>*> current_queue;
    std::queue<BinaryTreeNode<int>*> next_queue;
    current_queue.emplace(tree.get());

    while (!current_queue.empty()) {
      std::vector<int> level;

      while (!current_queue.empty()) {
        const auto node = current_queue.front();
        current_queue.pop();

        level.emplace_back(node->data);

        if (node->left)
          next_queue.emplace(node->left.get());
        if (node->right)
          next_queue.emplace(node->right.get());
      }

      levels.emplace_back(std::move(level));
      std::swap(current_queue, next_queue);
    }

    return levels;
  }
}

vector<vector<int>> BinaryTreeDepthOrder(
  const unique_ptr<BinaryTreeNode<int>>& tree
) {
  // TODO - you fill in here.
  return using_raw_pointers_and_two_queues(tree);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"tree"};
  return GenericTestMain(args, "tree_level_order.cc", "tree_level_order.tsv",
                         &BinaryTreeDepthOrder, DefaultComparator{},
                         param_names);
}
