#include <string>
#include <vector>

#include "test_framework/generic_test.h"
using std::string;
using std::vector;

namespace {

  int trivial_with_hash(const std::vector<std::string>& paragraph) {
    std::unordered_map<std::string, decltype(paragraph.begin())> word_to_prev_i;
    ptrdiff_t min_distance = std::numeric_limits<ptrdiff_t>::max();

    for (auto i = paragraph.begin(); i != paragraph.end(); ++i) {
      if (auto it = word_to_prev_i.find(*i); it != word_to_prev_i.end()) {
        min_distance = std::min(min_distance, i - it->second);
        it->second = i;
      }

      word_to_prev_i.emplace(*i, i);
    }

    return min_distance;
  }
}

int FindNearestRepetition(const vector<string>& paragraph) {
  // TODO - you fill in here.
  return trivial_with_hash(paragraph);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"paragraph"};
  return GenericTestMain(args, "nearest_repeated_entries.cc",
                         "nearest_repeated_entries.tsv", &FindNearestRepetition,
                         DefaultComparator{}, param_names);
}
