#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

int SearchFirstOfK(const vector<int>& A, int k) {
  size_t l = 0, r = A.size();

  while (l < r) {
    size_t m = l + (r - l) / 2;

    if (A[m] >= k)
      r = m;
    else
      l = m + 1;
  }

  return l < A.size() && A[l] == k ? l : -1;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"A", "k"};
  return GenericTestMain(args, "search_first_key.cc", "search_first_key.tsv",
                         &SearchFirstOfK, DefaultComparator{}, param_names);
}
