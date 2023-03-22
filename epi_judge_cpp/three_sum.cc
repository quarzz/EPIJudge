#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "test_framework/generic_test.h"
using std::unordered_set;
using std::vector;

namespace {
  // Passing
  bool brute_force(vector<int>& A, int t) {
    const size_t n = A.size();
    for (size_t i = 0; i < n; ++i)
      for (size_t j = i; j < n; ++j)
        for (size_t k = j; k < n; ++k)
          if (A[i] + A[j] + A[k] == t)
            return true;

    return false;
  }

  // Passing
  bool hashed(vector<int>& A, int t) {
    const unordered_set<int> h { A.cbegin(), A.cend() };
    const auto A_end = A.cend();

    for (auto i = A.cbegin(); i != A_end; ++i) {
      for (auto j = i; j != A_end; ++j) {
        const auto it = h.find(t - *i - *j);
        if (it != h.end())
          return true;
      }
    }

    return false;
  }

  // Passing
  bool recursive(
    vector<int>::const_iterator from,
    vector<int>::const_iterator to,
    int t
  ) {
    if (from >= to) return false;

    if (2 * *from + *(to - 1) > t)
      return recursive(from, to - 1, t);

    if (*from + 2 * *(to - 1) < t)
      return recursive(from + 1, to, t);

    if (std::binary_search(from, to, t - *from - *(to - 1)))
      return true;

    return recursive(from, to - 1, t) || recursive(from + 1, to, t);
  }

  // Precondition: A sorted
  bool two_sum(const vector<int>& A, int t) {
    int l = 0, r = A.size() - 1;

    while (l <= r) {
      const auto sum = A[l] + A[r];
      if (sum == t) return true;
      sum > t ? --r : ++l;
    }

    return false;
  }

  // Passing
  bool using_two_sum(vector<int>& A, int t) {
    std::sort(A.begin(), A.end());

    return std::any_of(A.cbegin(), A.cend(), [&A, t](int x) {
      return two_sum(A, t - x);
    });
  }
}

bool HasThreeSum(vector<int> A, int t) {
  return using_two_sum(A, t);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"A", "t"};
  return GenericTestMain(args, "three_sum.cc", "three_sum.tsv", &HasThreeSum,
                         DefaultComparator{}, param_names);
}
