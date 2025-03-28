#include <array>
#include <concepts>
#include <type_traits>
#include <vector>

#include "test_framework/generic_test.h"
#include "test_framework/test_failure.h"
#include "test_framework/timed_executor.h"
using std::vector;
enum class Color { kRed, kWhite, kBlue };

#define LOG(x) std::cout << #x << ": " << x << std::endl
// #define LOG(x) nullptr

template <typename T>
std::ostream& operator<<(std::ostream &os, const vector<T> &v) {
  for (const auto &x: v) {
    os << x << ' ';
  }
  return os << std::endl;
}

std::ostream& operator<<(std::ostream &os, const Color color) {
  return os << static_cast<int>(color);
}

namespace {
  // Passing
  // But not so elegant - change the order of rows and everything falls apart
  void contigous(size_t pivot_index, vector<Color> &A) {
    const auto pivot = A[pivot_index];
    const auto n = A.size();
    size_t i_e = 0, i_gt = 0;

    for (size_t i = 0; i < n; ++i) {
      if (A[i] < pivot) {
        const auto lt = A[i];
        A[i] = A[i_gt];
        A[i_gt++] = A[i_e];
        A[i_e++] = lt;
      } else if (A[i] == pivot) {
        std::swap(A[i], A[i_gt++]);
      } else /*(A[i] > pivot)*/ {
        ;
      }
    }
  }

  // Passing
  // But not so elegant - change the order of rows and everything falls apart
  void contigous_no_copy(size_t pivot_index, vector<Color> &A) {
    const auto n = A.size();
    std::swap(A[0], A[pivot_index]);
    size_t i_e = 1, i_gt = 1;

    for (size_t i = 1; i < n; ++i) {
      if (A[i] < A[0]) {
        const auto lt = A[i];
        A[i] = A[i_gt];
        A[i_gt++] = A[i_e];
        A[i_e++] = lt;
      } else if (A[i] == A[0]) {
        std::swap(A[i], A[i_gt++]);
      } else /*(A[i] > pivot)*/ {
        ;
      }
    }

    std::swap(A[0], A[i_e - 1]);
  }


  // Passing
  void discontigous(size_t pivot_index, vector<Color> &A) {
    const auto pivot = A[pivot_index];
    const size_t n = A.size();
    size_t i_e = 0, i_gt = n - 1;

    for (size_t i = 0; i <= i_gt ; ++i) {
      if (A[i] < pivot) {
        std::swap(A[i], A[i_e++]);
      } else if (A[i] == pivot) {
        ;
      } else /*A[i] > pivot*/ {
        std::swap(A[i--], A[i_gt--]);
      }
    }
  }

  // Passing
  void discontigous_no_copy(size_t pivot_index, vector<Color> &A) {
    const size_t n = A.size();
    std::swap(A[0], A[pivot_index]);
    size_t i_e = 1, i_gt = n - 1;

    for (size_t i = 1; i <= i_gt ; ++i) {
      if (A[i] < A[0]) {
        std::swap(A[i], A[i_e++]);
      } else if (A[i] == A[0]) {
        ;
      } else /*A[i] > pivot*/ {
        std::swap(A[i--], A[i_gt--]);
      }
    }

    std::swap(A[0], A[i_e - 1]);
  }
}

template<typename T, typename Fn>
requires std::regular_invocable<Fn, T> && std::is_integral_v<std::invoke_result_t<Fn, T>>
void partition(vector<T>& v,  const unsigned int ngroups, const Fn fn) {
  // Invariants:
  // fn(...) = 0: [0, ends[0])
  // fn(...) = i: [ends[i - 1], ends[i])
  // unclassified: [ends[ngroups - 1], size)
  const size_t size = v.size();
  vector<size_t> ends(ngroups, 0);
  while (ends[ngroups - 1] < size) {
    const auto cur_group = fn(v[ends[ngroups - 1]]) % ngroups;
    ++ends[ngroups - 1];
    for (auto i = ngroups - 1; i-- >= cur_group + 1;) {
      using std::swap;
      swap(v[ends[i]++], v[ends[i + 1] - 1]);
    }
  }
}

template<typename T, typename Predicate>
requires std::predicate<Predicate, const T&>
void stable_partition(vector<T>& v, const Predicate predicate) {
  // Invariant:
  // False group lies in [i + 1, j]
  // True group lies in [j + 1, size - 1]
  // Yet unclassified elements lie in [0, i]
  const int size = v.size();
  for (int i = size - 1, j = size - 1; i >= 0; --i) {
    if (predicate(v[i])) {
      using std::swap;
      swap(v[i], v[j--]);
    }
  }
}

void test_stable_partition() {
  {
    const vector original{1, 3, 5, 7};
    vector nums{1, 3, 5, 7};
    stable_partition(nums, [](int x) { return x % 2; });
    assert(nums == original);
  }

  {
    vector nums{1, 2, 3, 4, 5, 6, 7};
    stable_partition(nums, [](int x) { return x % 2; });
    const vector expected_tail{1, 3, 5, 7};
    const vector actual_tail((nums.end() - expected_tail.size()), nums.end());
    assert(actual_tail == expected_tail);
  }
}

void DutchFlagPartition(int pivot_index, vector<Color>* A_ptr) {
  const auto pivot = (*A_ptr)[pivot_index];
  partition(*A_ptr, 3, [pivot](const Color color) {
    return color < pivot ? 0 : color == pivot ? 1 : 2;
  });
  // contigous(pivot_index, *A_ptr);
}

void DutchFlagPartitionWrapper(TimedExecutor& executor, const vector<int>& A,
                               int pivot_idx) {
  vector<Color> colors;
  colors.resize(A.size());
  std::array<int, 3> count = {0, 0, 0};
  for (size_t i = 0; i < A.size(); i++) {
    count[A[i]]++;
    colors[i] = static_cast<Color>(A[i]);
  }
  Color pivot = colors[pivot_idx];

  executor.Run([&] { DutchFlagPartition(pivot_idx, &colors); });

  int i = 0;
  while (i < colors.size() && colors[i] < pivot) {
    count[static_cast<int>(colors[i])]--;
    ++i;
  }

  while (i < colors.size() && colors[i] == pivot) {
    count[static_cast<int>(colors[i])]--;
    ++i;
  }

  while (i < colors.size() && colors[i] > pivot) {
    count[static_cast<int>(colors[i])]--;
    ++i;
  }

  if (i != colors.size()) {
    throw TestFailure("Not partitioned after " + std::to_string(i) +
                      "th element");
  } else if (count != std::array<int, 3>{0, 0, 0}) {
    throw TestFailure("Some elements are missing from original array");
  }
}

int main(int argc, char* argv[]) {
  // vector<Color> colors {
  //   Color::kWhite,
  //   Color::kWhite,
  //   Color::kRed,
  //   Color::kBlue,
  // };
  // std::cout << colors;
  // DutchFlagPartition(1, &colors);
  // std::cout << colors;
  // return 0;
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"executor", "A", "pivot_idx"};
  return GenericTestMain(args, "dutch_national_flag.cc",
                         "dutch_national_flag.tsv", &DutchFlagPartitionWrapper,
                         DefaultComparator{}, param_names);
}
