#include <memory>

#include "list_node.h"
#include "test_framework/generic_test.h"
#include "test_framework/test_failure.h"
#include "test_framework/timed_executor.h"
using std::shared_ptr;

std::tuple<size_t, shared_ptr<ListNode<int>>>
get_tail(shared_ptr<ListNode<int>> head) {
  if (!head) return {0, nullptr};

  size_t length = 1;
  while (head->next) {
    ++length;
    head = head->next;
  }

  return {length, head};
};

shared_ptr<ListNode<int>> OverlappingNoCycleLists(
  shared_ptr<ListNode<int>> head1,
  shared_ptr<ListNode<int>> head2
) {

  auto [length1, tail1] = get_tail(head1);
  auto [length2, tail2] = get_tail(head2);

  if (tail1 != tail2) return nullptr;

  if (length1 > length2) {
    using std::swap;
    swap(length1, length2);
    swap(head1, head2);
  }

  for (auto i = length2 - length1; i--;) {
    head2 = head2->next;
  }

  while (head1 != head2) {
    head1 = head1->next;
    head2 = head2->next;
  }

  return head1;
}

void OverlappingNoCycleListsWrapper(TimedExecutor& executor,
                                    shared_ptr<ListNode<int>> l0,
                                    shared_ptr<ListNode<int>> l1,
                                    shared_ptr<ListNode<int>> common) {
  if (common) {
    if (l0) {
      auto i = l0;
      while (i->next) {
        i = i->next;
      }
      i->next = common;
    } else {
      l0 = common;
    }

    if (l1) {
      auto i = l1;
      while (i->next) {
        i = i->next;
      }
      i->next = common;
    } else {
      l1 = common;
    }
  }

  auto result = executor.Run([&] { return OverlappingNoCycleLists(l0, l1); });

  if (result != common) {
    throw TestFailure("Invalid result");
  }
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"executor", "l0", "l1", "common"};
  return GenericTestMain(
      args, "do_terminated_lists_overlap.cc", "do_terminated_lists_overlap.tsv",
      &OverlappingNoCycleListsWrapper, DefaultComparator{}, param_names);
}
