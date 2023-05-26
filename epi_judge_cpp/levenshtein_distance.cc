#include <string>
#include <vector>

#include "test_framework/generic_test.h"
using std::string;
using std::vector;

#define log(x) std::cout << #x << ": " << (x) << std::endl

namespace {
  struct PairHash {
      template <typename T, typename U>
      std::size_t operator()(const std::pair<T, U>& p) const {
          auto h1 = std::hash<T>{}(p.first);
          auto h2 = std::hash<U>{}(p.second);
          return h1 ^ h2;
      }
  };

  std::map<std::pair<int, int>, int> memo;

  int top_down_map_memo(const string& A, const string& B, int i = 0, int j = 0) {
    auto& cur_memo = memo[{ i, j }];
    if (cur_memo) return cur_memo;

    if (i == A.size()) {
      cur_memo = B.size() - j;
    } else if (j == B.size()) {
      cur_memo = A.size() - i;
    } else if (A[i] == B[j]) {
      cur_memo = top_down_map_memo(A, B, i + 1, j + 1);
    } else {
      cur_memo = 1 + std::min({
        top_down_map_memo(A, B, i + 1, j + 1),
        top_down_map_memo(A, B, i, j + 1),
        top_down_map_memo(A, B, i + 1, j)
      });
    }

    return cur_memo;
  }

  int top_down_vector_memo(
    const string& A,
    const string& B,
    std::vector<std::vector<int>> memo,
    int i = 0,
    int j = 0
  ) {
    auto& cur_memo = memo[i][j];

    if (cur_memo) return cur_memo;

    if (i == A.size()) {
      cur_memo = B.size() - j;
    } else if (j == B.size()) {
      cur_memo = A.size() - i;
    } else if (A[i] == B[j]) {
      cur_memo = top_down_map_memo(A, B, i + 1, j + 1);
    } else {
      cur_memo = 1 + std::min({
        top_down_map_memo(A, B, i + 1, j + 1),
        top_down_map_memo(A, B, i, j + 1),
        top_down_map_memo(A, B, i + 1, j)
      });
    }

    return cur_memo;
  }

  int top_down_hash_memo(
    const string& A,
    const string& B,
    std::unordered_map<std::pair<int, int>, int, PairHash>& memo,
    int i = 0,
    int j = 0
  ) {
    auto& cur_memo = memo[{ i, j }];

    if (cur_memo) return cur_memo;

    if (i == A.size()) {
      cur_memo = B.size() - j;
    } else if (j == B.size()) {
      cur_memo = A.size() - i;
    } else if (A[i] == B[j]) {
      cur_memo = top_down_map_memo(A, B, i + 1, j + 1);
    } else {
      cur_memo = 1 + std::min({
        top_down_map_memo(A, B, i + 1, j + 1),
        top_down_map_memo(A, B, i, j + 1),
        top_down_map_memo(A, B, i + 1, j)
      });
    }

    return cur_memo;
  }

  int bottom_up_vector_memo(const std::string& A, const std::string& B) {
    std::vector<std::vector<int>> memo(A.size() + 1, std::vector<int>(B.size() + 1, 0));
    for (int i = 0; i < A.size(); ++i)
      memo[i][B.size()] = A.size() - i;
    for (int j = 0; j < B.size(); ++j)
      memo[A.size()][j] = B.size() - j;

    for (int i = A.size() - 1; i >= 0; --i) {
      for (int j = B.size() - 1; j >= 0; --j) {
        memo[i][j] = A[i] == B[j]
          ? memo[i + 1][j + 1]
          : 1 + std::min({ memo[i][j + 1], memo[i + 1][j + 1], memo[i + 1][j] });
      }
    }

    return memo[0][0];
  }

  int bottom_up_raw_memo(const std::string& A, const std::string& B) {
    auto raw = new int[(A.size() + 1) * (B.size() + 1)];
    auto memo = new int*[A.size() + 1];
    for (int i = 0; i <= A.size(); ++i)
      memo[i] = &raw[i * (B.size() + 1)];

    for (int i = 0; i < A.size(); ++i)
      memo[i][B.size()] = A.size() - i;
    for (int j = 0; j < B.size(); ++j)
      memo[A.size()][j] = B.size() - j;
    memo[A.size()][B.size()] = 0;

    for (int i = A.size() - 1; i >= 0; --i) {
      for (int j = B.size() - 1; j >= 0; --j) {
        memo[i][j] = A[i] == B[j]
          ? memo[i + 1][j + 1]
          : 1 + std::min({ memo[i][j + 1], memo[i + 1][j + 1], memo[i + 1][j] });
      }
    }

    const auto res = memo[0][0];

    delete[] memo;
    delete[] raw;

    return res;
  }
}

int LevenshteinDistance(const string& A, const string& B) {
  // TODO - you fill in here.
  memo.clear();
  // return using_map_memo(A, B);
  // std::vector<std::vector<int>> memo(A.size(), vector<int>(B.size()));
  // return top_down_vector_memo(A, B, memo);
  // std::unordered_map<std::pair<int, int>, int, PairHash> memo;
  // return using_hash_memo(A, B, memo);
  // return bottom_up_vector_memo(A, B);
  return bottom_up_raw_memo(A, B);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"A", "B"};
  return GenericTestMain(args, "levenshtein_distance.cc",
                         "levenshtein_distance.tsv", &LevenshteinDistance,
                         DefaultComparator{}, param_names);
}
