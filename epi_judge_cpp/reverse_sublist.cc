#include "list_node.h"
#include "test_framework/generic_test.h"

#define log(x) std::cout << #x << ": " << x << std::endl

namespace {
  struct ListWrapper {
    shared_ptr<ListNode<int>> first;
    shared_ptr<ListNode<int>> last;
  };

  shared_ptr<ListNode<int>> next_node;

  ListWrapper recursive_impl(shared_ptr<ListNode<int>> list, int length) {
    if (!length) {
      next_node = list->next;
      return { list, list };
    }

    auto reversed_tail = recursive_impl(list->next, length - 1);
    reversed_tail.last->next = list;
    list->next = nullptr;

    return { reversed_tail.first, list };
  }

  // Passing
  shared_ptr<ListNode<int>> recursive(
    shared_ptr<ListNode<int>> L,
    const int from,
    const int to
  ) {
    const auto head = std::make_shared<ListNode<int>>(0, L);
    auto prev_node = head;

    // Skip first from - 1
    for (int i = 1;  i < from; ++i)
      prev_node = prev_node->next;

    // Reverse from from to to
    auto cur_node = prev_node->next;
    auto reversed_tail = recursive_impl(prev_node->next, to - from);
    prev_node->next = reversed_tail.first;

    // Restore from from + 1 to the end
    reversed_tail.last->next = next_node;

    return head->next;
  }

  // Passing
  shared_ptr<ListNode<int>> iterative(
    shared_ptr<ListNode<int>> L,
    const int from,
    const int to
  ) {
    const auto head = std::make_shared<ListNode<int>>(0, L);
    auto prev_node = head;

    // Skip first from - 1
    for (int i = 1;  i < from; ++i)
      prev_node = prev_node->next;

    // Reverse node from from to to
    auto cur_node = prev_node->next;
    auto next_node = cur_node->next;
    ListWrapper reversed_sublist { cur_node, cur_node };

    for (int i = 0; i < to - from; ++i) {
      cur_node = next_node;
      next_node = cur_node->next;
      cur_node->next = reversed_sublist.first;
      reversed_sublist.first = cur_node;
    }

    prev_node->next = reversed_sublist.first;
    reversed_sublist.last->next = next_node;

    return head->next;
  }

  shared_ptr<ListNode<int>> iterative_smarter(
    shared_ptr<ListNode<int>> L,
    int from,
    int to
  ) {
    const auto head = make_shared<ListNode<int>>(0, L);
    auto sub_head = head;
    for (int i = 1; i < from; ++i)
      sub_head = sub_head->next;

    auto sub_iter = sub_head->next;

    while (from++ < to) {
      auto cur = sub_iter->next;
      sub_iter->next = cur->next;
      cur->next = sub_head->next;
      sub_head->next = cur;
    }

    return head->next;
  }
}

shared_ptr<ListNode<int>> ReverseSublist(shared_ptr<ListNode<int>> L, int start,
                                         int finish) {
  return start ? iterative_smarter(L, start, finish) : L;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"L", "start", "finish"};
  return GenericTestMain(args, "reverse_sublist.cc", "reverse_sublist.tsv",
                         &ReverseSublist, DefaultComparator{}, param_names);
}
