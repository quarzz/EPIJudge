#include <queue>
#include <string>
#include <unordered_set>

#include "test_framework/generic_test.h"
using std::string;
using std::unordered_set;

#define log(x) std::cout << #x << ": " << (x) << std::endl

namespace {
  using It = std::unordered_set<std::string>::iterator;

  struct IteratorHash {
    std::size_t operator()(const It& it) const {
      return reinterpret_cast<size_t>(&*it);
    }
  };

  // O(D.size() * s.size())
  inline int iterate_on_candidates(
    std::unordered_set<std::string> D,
    const std::string& s,
    const std::string& t
  ) {
    if (!D.count(s) || !D.count(t))
      return -1;

    std::queue<std::pair<std::string, int>> q;
    q.push({ s, 0 });
    D.erase(s);

    // Invariant 1: cur is never in D
    while (!q.empty()) {
      auto [cur, distance] = q.front();
      q.pop();

      for (size_t i = 0; i < cur.size(); ++i) {
        const auto i_char = cur[i];

        for (auto j = 'a'; j <= 'z'; ++j) {
          cur[i] = j;
          if (const auto it = D.find(cur); it != D.end()) {
            if (cur == t) return distance + 1;

            q.push({ cur, distance + 1 });
            D.erase(it);
          }
        }

        cur[i] = i_char;
      }
    }

    return -1;
  }
}

int TransformString(unordered_set<string> D, const string& s, const string& t) {
  return iterate_on_candidates(D, s, t);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"D", "s", "t"};
  return GenericTestMain(args, "string_transformability.cc",
                         "string_transformability.tsv", &TransformString,
                         DefaultComparator{}, param_names);
}
