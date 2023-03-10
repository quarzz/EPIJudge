#include "list_node.h"
#include "test_framework/generic_test.h"

#include <memory>

namespace {
  void AppendNode(
    shared_ptr<ListNode<int>> *where,
    shared_ptr<ListNode<int>> *what
  ) {
    (*where)->next = *what;
    *where = (*where)->next;
    *what = (*what)->next;
  }
}

shared_ptr<ListNode<int>> MergeTwoSortedLists(
  shared_ptr<ListNode<int>> L1,
  shared_ptr<ListNode<int>> L2
) {
  auto dummy_head = std::make_shared<ListNode<int>>();
  auto tail = dummy_head;

  while (L1 && L2)
    AppendNode(&tail, L1->data < L2->data ? &L1 : &L2);
  tail->next = L1 ? L1 : L2;

  return dummy_head->next;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"L1", "L2"};
  return GenericTestMain(args, "sorted_lists_merge.cc",
                         "sorted_lists_merge.tsv", &MergeTwoSortedLists,
                         DefaultComparator{}, param_names);
}
