#include <map>
#include <unordered_map>
#include <vector>

#include "test_framework/generic_test.h"

using std::map;
using std::pair;
using std::unordered_map;
using std::vector;

#define log(x) std::cout << #x << ": " << (x) << std::endl
// #define log(x) nullptr

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};


namespace {
  // Passing
  // Faster than tree
  int recursive_hash(
    int S,
    const vector<int>& w,
    int i,
    unordered_map<pair<int, int>, int, pair_hash>& cache
  ) {
    // Check cache
    const auto it = cache.find({ i, S });
    if (it != cache.end()) return it->second;

    // Recursion base
    if (i == 1) {
      const auto res = S % w[i - 1] ? 0 : 1;
      cache[{ i, S }] = res;
      return res;
    }

    int result = 0;
    int w_i = w[i - 1];

    for (int j = 0; S - j * w_i >= 0; ++j)
      result += recursive_hash(S - j * w_i, w, i - 1, cache);

    cache[{ i, S }] = result;
    return result;
  }

  // Passing
  int recursive_tree(
    int S,
    const vector<int>& w,
    int i,
    map<pair<int, int>, int>& cache
  ) {
    // Check cache
    const auto it = cache.find({ i, S });
    if (it != cache.end()) return it->second;

    // Recursion base
    if (i == 1) {
      const auto res = S % w[i - 1] ? 0 : 1;
      cache[{ i, S }] = res;
      return res;
    }

    int result = 0;
    int w_i = w[i - 1];

    for (int j = 0; S - j * w_i >= 0; ++j)
      result += recursive_tree(S - j * w_i, w, i - 1, cache);

    cache[{ i, S }] = result;
    return result;
  }

  // Passing
  // Faster than tree or hash
  int recursive_vector(
    int S,
    const vector<int>& w,
    int i,
    vector<vector<int>>& cache
  ) {
    // Check cache
    if (cache[i][S] >= 0) return cache[i][S];

    // Recursion base
    if (i == 1) {
      const auto res = S % w[i - 1] ? 0 : 1;
      cache[i][S] = res;
      return res;
    }

    int result = 0;
    int w_i = w[i - 1];

    for (int j = 0; S - j * w_i >= 0; ++j)
      result += recursive_vector(S - j * w_i, w, i - 1, cache);

    cache[i][S] = result;
    return result;
  }

  // Passing
  int iterative_vector(int S, const vector<int> &w) {
    vector<vector<int>> A(w.size(), vector<int>(S + 1, 0));

    for (size_t j = 0; j <= S; ++j)
      A[0][j] = j % w[0] ? 0 : 1;

    for (size_t i = 1; i < w.size(); ++i) {
      for (int j = 0; j < w[i] && j <= S; ++j)
        A[i][j] = A[i - 1][j];

      for (int j = w[i]; j <= S; ++j)
        A[i][j] = A[i][j - w[i]] + A[i - 1][j];
    }

    return A[w.size() - 1][S];
  }

  // Passing. The fastest of all my solutions
  int iterative_vector_less_space(int S, const vector<int> &w) {
    vector<int> A(S + 1, 0);

    for (size_t j = 0; j <= S; ++j)
      A[j] = j % w[0] ? 0 : 1;

    for (size_t i = 1; i < w.size(); ++i)
      for (int j = w[i]; j <= S; ++j)
        A[j] += A[j - w[i]];

    return A[S];
  }
}

int NumCombinationsForFinalScore(
  int final_score,
  const vector<int>& individual_play_scores
  ) {
    return iterative_vector_less_space(final_score, individual_play_scores);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"final_score", "individual_play_scores"};
  return GenericTestMain(args, "number_of_score_combinations.cc",
                         "number_of_score_combinations.tsv",
                         &NumCombinationsForFinalScore, DefaultComparator{},
                         param_names);
}
