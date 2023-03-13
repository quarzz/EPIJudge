#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

namespace {
  // Passing
  vector<int> impl_no_if_empty(const vector<int>& A, const vector<int>& B) {
    vector<int> res;
    auto i_a = A.cbegin();
    auto i_b = B.cbegin();

    while (i_a != A.cend() && i_b != B.cend()) {
      if (*i_a < *i_b) {
        ++i_a;
      } else if (*i_a > *i_b) {
        ++i_b;
      } else {
        res.emplace_back(*i_a);
        ++i_a;
        ++i_b;
        break;
      }
    }

    while (i_a != A.cend() && i_b != B.cend()) {
      if (*i_a < *i_b) {
        ++i_a;
      } else if (*i_a > *i_b) {
        ++i_b;
      } else {
        if (res.back() != *i_a)
          res.emplace_back(*i_a);

        ++i_a;
        ++i_b;
      }
    }

    return res;
  }

  // Passing
  // I expected to notice decrease in performance for additional if check
  //   but I guess branch prediction unit is too good nowadays
  vector<int> impl_if_empty(const vector<int>& A, const vector<int>& B) {
    vector<int> res;
    auto i_a = A.cbegin();
    auto i_b = B.cbegin();

    while (i_a != A.cend() && i_b != B.cend()) {
      if (*i_a < *i_b) {
        ++i_a;
      } else if (*i_a > *i_b) {
        ++i_b;
      } else {
        if (res.empty() || res.back() != *i_a)
          res.emplace_back(*i_a);
        ++i_a;
        ++i_b;
      }
    }

    return res;
  }
};

vector<int> IntersectTwoSortedArrays(
  const vector<int>& A,
  const vector<int>& B
) {
  return impl_no_if_empty(A, B);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"A", "B"};
  return GenericTestMain(
      args, "intersect_sorted_arrays.cc", "intersect_sorted_arrays.tsv",
      &IntersectTwoSortedArrays, DefaultComparator{}, param_names);
}
