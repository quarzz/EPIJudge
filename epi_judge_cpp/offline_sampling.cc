#include <algorithm>
#include <functional>
#include <iterator>
#include <random>
#include <vector>

#include "test_framework/generic_test.h"
#include "test_framework/random_sequence_checker.h"
#include "test_framework/timed_executor.h"
using std::bind;
using std::vector;

#define log(x) std::cout << #x << ": " << (x) << std::endl

namespace {
  void multiple_distrib(int k, vector<int>* A_ptr) {
    auto& A = *A_ptr;
    std::random_device rd;
    std::mt19937 gen { rd() };

    const size_t n = A.size();
    // log(k); log(n);
    for (size_t i = 0; i < k; ++i) {
      std::uniform_int_distribution<> distrib(i, n - 1);
      std::swap(A[i], A[distrib(gen)]);
    }

    A.erase(A.cbegin() + k, A.cend());
  }

  void one_distrib(int k, vector<int>* A_ptr) {
    auto& A = *A_ptr;
    const size_t n = A.size();
    std::random_device rd;
    std::mt19937 gen { rd() };
    std::uniform_int_distribution<> distrib(0, n - 1);

    for (size_t i = 0; i < k; ++i) {
      const auto j = n - 1 - distrib(gen) % (n - i);
      std::swap(A[i], A[j]);
    }

    A.erase(A.cbegin() + k, A.cend());
  }
}

void RandomSampling(int k, vector<int>* A_ptr) {
  one_distrib(k, A_ptr);
}
bool RandomSamplingRunner(TimedExecutor& executor, int k, vector<int> A) {
  using namespace test_framework;
  vector<vector<int>> results;

  executor.Run([&] {
    for (int i = 0; i < 100000; ++i) {
      RandomSampling(k, &A);
      results.emplace_back(begin(A), begin(A) + k);
    }
  });

  int total_possible_outcomes = BinomialCoefficient(A.size(), k);
  sort(begin(A), end(A));
  vector<vector<int>> combinations;
  for (int i = 0; i < BinomialCoefficient(A.size(), k); ++i) {
    combinations.emplace_back(ComputeCombinationIdx(A, A.size(), k, i));
  }
  vector<int> sequence;
  for (vector<int> result : results) {
    sort(begin(result), end(result));
    sequence.emplace_back(
        distance(begin(combinations),
                 find(begin(combinations), end(combinations), result)));
  }
  return CheckSequenceIsUniformlyRandom(sequence, total_possible_outcomes,
                                        0.01);
}

void RandomSamplingWrapper(TimedExecutor& executor, int k,
                           const vector<int>& A) {
  RunFuncWithRetries(
      bind(RandomSamplingRunner, std::ref(executor), k, std::cref(A)));
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"executor", "k", "A"};
  return GenericTestMain(args, "offline_sampling.cc", "offline_sampling.tsv",
                         &RandomSamplingWrapper, DefaultComparator{},
                         param_names);
}
