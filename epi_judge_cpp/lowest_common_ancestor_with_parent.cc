#include "binary_tree_with_parent_prototype.h"
#include "test_framework/binary_tree_utils.h"
#include "test_framework/generic_test.h"
#include "test_framework/test_failure.h"
#include "test_framework/timed_executor.h"

#include <unordered_set>

namespace {
  using Node = BinaryTreeNode<int>;
  using NodePtr = std::unique_ptr<Node>;

  // Passing
  auto using_hash(const NodePtr& node1_ptr, const NodePtr& node2_ptr) {
    std::unordered_set<Node*> h;

    auto node1 = node1_ptr.get();
    while (node1) {
      h.emplace(node1);
      node1 = node1->parent;
    }

    auto node2 = node2_ptr.get();
    while (node2) {
      if (auto it = h.find(node2); it != h.end())
        return node2;

      node2 = node2->parent;
    }

    return node2;
  }

  auto get_height(Node* node) {
    auto height = 0;

    while (node) {
      node = node->parent;
      ++height;
    }

    return height;
  }

  // Passing
  auto using_heights(const NodePtr& node1_ptr, const NodePtr& node2_ptr) {
    const auto node1 = node1_ptr.get();
    const auto node2 = node2_ptr.get();
    const auto height1 = get_height(node1);
    const auto height2 = get_height(node2);

    // in Rust this would be much more elegant...
    // well, the solution from book just using node1 being the nearest as
    //   an invariant and just swaps if its not initially
    // but I still want to be more verbose about this. Maybe I shouldn't
    auto [nearest, farest] = height1 < height2 ?
      std::make_tuple(node1, node2) : std::make_tuple(node2, node1);

    for (int i = 0; i < std::abs(height2 - height1); ++i)
      farest = farest->parent;

    while (nearest) {
      if (nearest == farest) return nearest;
      nearest = nearest->parent;
      farest = farest->parent;
    }

    return nearest;
  }
}

BinaryTreeNode<int>* Lca(const unique_ptr<BinaryTreeNode<int>>& node0,
                         const unique_ptr<BinaryTreeNode<int>>& node1) {
  // TODO - you fill in here.
  return using_heights(node0, node1);
}
int LcaWrapper(TimedExecutor& executor,
               const unique_ptr<BinaryTreeNode<int>>& tree, int key0,
               int key1) {
  const unique_ptr<BinaryTreeNode<int>>& node0 = MustFindNode(tree, key0);
  const unique_ptr<BinaryTreeNode<int>>& node1 = MustFindNode(tree, key1);

  auto result = executor.Run([&] { return Lca(node0, node1); });

  if (!result) {
    throw TestFailure("Result can not be nullptr");
  }
  return result->data;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"executor", "tree", "key0", "key1"};
  return GenericTestMain(args, "lowest_common_ancestor_with_parent.cc",
                         "lowest_common_ancestor.tsv", &LcaWrapper,
                         DefaultComparator{}, param_names);
}
