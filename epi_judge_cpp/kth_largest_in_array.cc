#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

#define log(x) std::cout << #x << ": " << (x) << std::endl

namespace {
  // Passing
  int naive_using_sort(int k, std::vector<int>& A) {
    std::sort(A.begin(), A.end());
    return A[A.size() - k];
  }

  // Passing
  int naive_using_heap(int k, const std::vector<int>& A) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> largest_nums {
      A.cbegin(), A.cbegin() + k
    };

    for (auto i = A.cbegin() + k; i != A.cend(); ++i) {
      if (*i > largest_nums.top()) {
        largest_nums.pop();
        largest_nums.push(*i);
      }
    }

    return largest_nums.top();
  }

  // Passing
  int using_heap_inplace(int k, std::vector<int>& A) {
    std::make_heap(A.begin(), A.begin() + k, std::greater<int> {});

    const auto A_end = A.end();
    for (auto i = A.begin() + k; i != A_end; ++i) {
      if (A.front() < *i) {
        std::pop_heap(A.begin(), A.begin() + k, std::greater<int> {});
        A[k - 1] = *i;
        std::push_heap(A.begin(), A.begin() + k, std::greater<int> {});
      }
    }

    return A.front();
  }

  // Of course, all the variants above can be speed up by switching to raw ptrs
  //   But I want to find conceptually new approach

  struct PivotResult {
    // const in i_gt = 0;
    std::vector<int>::iterator i_eq;
    std::vector<int>::iterator i_lt;
  };

  PivotResult pivot(
    const std::vector<int>::iterator first,
    const std::vector<int>::iterator last,
    const int pivot_val
  ) {
    auto i_eq = first, i_lt = last;

    for (auto i = first; i != i_lt;) {
      if (*i > pivot_val)
        std::swap(*i_eq++, *i++);
      else if (*i < pivot_val)
        std::swap(*i, *(--i_lt));
      else
        ++i;
    }

    return { i_eq, i_lt };
  }

  // Passing
  int using_pivot(
    std::vector<int>::iterator first,
    std::vector<int>::iterator last,
    int k
  ) {
    const auto pivot_val = first[std::rand() % (last - first)];
    const auto [i_eq, i_lt] = pivot(first, last, pivot_val);

    if (first + k <= i_eq)
      return using_pivot(first, i_eq, k);
    if (first + k > i_lt)
      return using_pivot(i_lt, last, k - (i_lt - first));

    return pivot_val;
  }
}

// The numbering starts from one, i.e., if A = [3, 1, -1, 2] then
// FindKthLargest(1, A) returns 3, FindKthLargest(2, A) returns 2,
// FindKthLargest(3, A) returns 1, and FindKthLargest(4, A) returns -1.
int FindKthLargest(int k, vector<int>* A_ptr) {
  // TODO - you fill in here.
  std::srand(std::time(nullptr));
  return using_pivot(A_ptr->begin(), A_ptr->end(), k);
  // return naive_using_sort(k, *A_ptr);
}
int FindKthLargestWrapper(int k, vector<int>& A) {
  return FindKthLargest(k, &A);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"k", "A"};
  return GenericTestMain(args, "kth_largest_in_array.cc",
                         "kth_largest_in_array.tsv", &FindKthLargestWrapper,
                         DefaultComparator{}, param_names);
}
