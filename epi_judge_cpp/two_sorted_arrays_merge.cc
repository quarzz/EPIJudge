#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

// #define log(x) std::cout << #x << ": " << (x) << std::endl
// #define log(x) nullptr

namespace {
  void insert_backwards_i(vector<int>& A, const vector<int>& B) {
    long insert_i = A.size() - 1;
    long a_i = insert_i - B.size();
    long b_i = B.size() - 1;

    while (a_i >= 0 && b_i >= 0)
      A[insert_i--] = A[a_i] > B[b_i] ? A[a_i--] : B[b_i--];

    log(insert_i);
    log(b_i);

    while (b_i >= 0)
      A[insert_i--] = B[b_i--];
  }

  // Slower...
  void insert_backwards_it(vector<int>& A, const vector<int>& B) {
    const auto a_begin = A.begin() - 1;
    const auto b_begin = B.begin() - 1;

    auto insert_it = A.end() - 1;
    auto a_it = insert_it - B.size();
    auto b_it = B.end() - 1;

    while (a_it != a_begin && b_it != b_begin)
      *(insert_it--) = *a_it > *b_it ? *(a_it--) : *(b_it--);

    while (b_it != b_begin)
      *(insert_it--) = *(b_it--);
  }

  // Omg, so slow...
  void insert_backwards_rit(vector<int>& A, const vector<int>& B) {
    const auto a_rend = A.rend();
    const auto b_rend = B.rend();

    auto insert_it = A.rbegin();
    auto a_it = A.rbegin() + B.size();
    auto b_it = B.rbegin();

    while (a_it != a_rend && b_it != b_rend)
      *(insert_it++) = *a_it > *b_it ? *(a_it++) : *(b_it++);

    while (b_it != b_rend)
      *(insert_it++) = *(b_it++);
  }
}

void MergeTwoSortedArrays(vector<int>& A, int m, const vector<int>& B, int n) {
  // TODO - you fill in here.
  insert_backwards_i(A, B);
  // insert_backwards_it(A, B);
  // insert_backwards_rit(A, B);
}
vector<int> MergeTwoSortedArraysWrapper(vector<int> A, int m,
                                        const vector<int>& B, int n)
{
  log(A.size());
  log(m);
  log(B.size());
  log(n);
  MergeTwoSortedArrays(A, m, B, n);
  return A;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"A", "m", "B", "n"};
  return GenericTestMain(
      args, "two_sorted_arrays_merge.cc", "two_sorted_arrays_merge.tsv",
      &MergeTwoSortedArraysWrapper, DefaultComparator{}, param_names);
}
