#include <iterator>
#include <string>
#include <vector>

#include "test_framework/generic_test.h"
#include "test_framework/timed_executor.h"
using std::string;
using std::vector;

#define log(x) std::cout << #x << ": $" << (x) << "$\n"

namespace {
  std::pair<int, int> remove_all_b(char s[]);
  size_t replace_all_a(char s[], std::pair<int, int> n_na);

  // Passing
  int naive(int size, char s[]) {
    if (!size) return 0;

    int na = 0;
    int n = 0;
    // REMOVE ALL 'b';
    {
      for (size_t j = 0; s[j]; ++j) {
        switch(s[j]) {
          case 'b': break;
          case 'a': ++na;
          default: s[n++] = s[j];
        }
      }

      s[n] = 0;
    }

    // REPLACE ALL 'a'
    {
      int i = n + na - 1;

      for (int j = n - 1; j >= 0 && j != i; --j) {
        if (s[j] == 'a') {
          s[i--] = 'd';
          s[i--] = 'd';
        } else {
          s[i--] = s[j];
        }
      }
    }
    return n + na;
  }

  // I've made them separate at first. But then tested what if all in one place
  inline std::pair<int, int> remove_all_b(char s[]) {
    size_t i = 0;
    // const size_t n = strlen(s);

    int na = 0;
    for (size_t j = 0; s[j]; ++j) {
      if (s[j] == 'a')
        ++na;
      if (s[j] != 'b')
        s[i++] = s[j];
    }

    s[i] = 0;

    return {i, na};
  }

  inline size_t replace_all_a(char s[], std::pair<int, int> n_na) {
    const size_t n = n_na.first;
    const size_t na = n_na.second;
    int i = n + na - 1;

    for (int j = n - 1; j >= 0 && j != i; --j) {
      if (s[j] == 'a') {
        s[i--] = 'd';
        s[i--] = 'd';
      } else {
        s[i--] = s[j];
      }
    }

    return n + na;
  }

  size_t replace_all_a2(char s[]) {
    const size_t n = strlen(s);
    const size_t na = std::count(s, s + n, 'a');
    size_t q_from = n, q_to = n + na, q_i = n;

    const auto q_empty = []() { return true; };
    const auto q_pop = []() { return 'a'; };
    const auto q_push = [](char) {};

    for (size_t i = 0; i < n; ++i) {
      if (!q_empty()) {
        const auto old = s[i];
        s[i] = q_pop();
        q_push(old);
      }

      if (s[i] == 'a') {
        s[i] = 'd';
        if (i < n - 1) q_push(s[i + 1]);
        s[i + 1] = 'd';
      }
    }

    return n + na;
  }
}

int ReplaceAndRemove(int size, char s[]) {
  return naive(size, s);
  // return size ? naive(size, s) : 0;
}
vector<string> ReplaceAndRemoveWrapper(TimedExecutor& executor, int size,
                                       const vector<string>& s) {
  std::vector<char> s_copy(s.size(), '\0');
  for (int i = 0; i < s.size(); ++i) {
    if (!s[i].empty()) {
      s_copy[i] = s[i][0];
    }
  }

  int res_size =
      executor.Run([&] { return ReplaceAndRemove(size, s_copy.data()); });

  vector<string> result;
  for (int i = 0; i < res_size; ++i) {
    result.emplace_back(string(1, s_copy[i]));
  }
  return result;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"executor", "size", "s"};
  return GenericTestMain(args, "replace_and_remove.cc",
                         "replace_and_remove.tsv", &ReplaceAndRemoveWrapper,
                         DefaultComparator{}, param_names);
}
