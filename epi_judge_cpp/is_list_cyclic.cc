#include <memory>
#include <optional>

#include "list_node.h"
#include "test_framework/generic_test.h"
#include "test_framework/test_failure.h"
#include "test_framework/timed_executor.h"
using std::shared_ptr;

namespace {
  auto get_next(std::shared_ptr<ListNode<int>> head, int count = 1) {
    while (head && count--)
      head = head->next;

    return head;
  }

  auto get_cycle_length(const std::shared_ptr<ListNode<int>>& head) {
    auto i = 1;
    auto it1 = head, it2 = get_next(head);

    while (it2 && it1 != it2) {
      it1 = it1->next;
      it2 = get_next(it2, 2);
      ++i;
    }

    return it2 ? std::make_optional(i) : std::nullopt;
  }

  auto get_cycle_start(
    const std::shared_ptr<ListNode<int>>& head,
    int cycle_length
  ) {
    auto it1 = head;
    auto it2 = get_next(it1, cycle_length);

    while (it1 != it2) {
      it1 = it1->next;
      it2 = it2->next;
    }

    return it1;
  }

  // Passing
  auto impl(const std::shared_ptr<ListNode<int>>& head) {
    const auto cycle_length = get_cycle_length(head);
    return cycle_length ? get_cycle_start(head, *cycle_length) : nullptr;
  }
}

shared_ptr<ListNode<int>> HasCycle(const shared_ptr<ListNode<int>>& head) {
  // TODO - you fill in here.
  return impl(head);
}
void HasCycleWrapper(TimedExecutor& executor,
                     const shared_ptr<ListNode<int>>& head, int cycle_idx) {
  int cycle_length = 0;
  if (cycle_idx != -1) {
    if (!head) {
      throw std::runtime_error("Can't cycle empty list");
    }
    shared_ptr<ListNode<int>> cycle_start, cursor = head;
    while (cursor->next) {
      if (cursor->data == cycle_idx) {
        cycle_start = cursor;
      }
      cursor = cursor->next;
      cycle_length += !!cycle_start;
    }
    if (cursor->data == cycle_idx) {
      cycle_start = cursor;
    }
    if (!cycle_start) {
      throw std::runtime_error("Can't find a cycle start");
    }
    cursor->next = cycle_start;
    cycle_length++;
  }
  shared_ptr<ListNode<int>> result =
      executor.Run([&] { return HasCycle(head); });

  if (cycle_idx == -1) {
    if (result != nullptr) {
      throw TestFailure("Found a non-existing cycle");
    }
  } else {
    if (result == nullptr) {
      throw TestFailure("Existing cycle was not found");
    }

    auto cursor = result;
    do {
      cursor = cursor->next;
      cycle_length--;
      if (!cursor || cycle_length < 0) {
        throw TestFailure(
            "Returned node does not belong to the cycle or is not the "
            "closest node to the head");
      }
    } while (cursor != result);

    if (cycle_length != 0) {
      throw TestFailure(
          "Returned node does not belong to the cycle or is not the closest "
          "node to the head");
    }
  }
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"executor", "head", "cycle_idx"};
  return GenericTestMain(args, "is_list_cyclic.cc", "is_list_cyclic.tsv",
                         &HasCycleWrapper, DefaultComparator{}, param_names);
}
