#include <tuple>
#include <vector>

#include "test_framework/fmt_print.h"
#include "test_framework/generic_test.h"
#include "test_framework/serialization_traits.h"

using std::vector;

struct PairedTasks {
  int task_1, task_2;
};

namespace {
  // Passing
  vector<PairedTasks> greedy(vector<int>& task_durations) {
    vector<PairedTasks> res;

    const auto n = task_durations.size();
    std::sort(task_durations.begin(), task_durations.end());
    for (size_t i = 0; i < n / 2; ++i)
      res.push_back({task_durations[i], task_durations[n - 1 - i]});

    return res;
  }

  // Passing
  vector<PairedTasks> cache_friendly(vector<int>& task_durations) {
    const auto n = task_durations.size();
    const auto k = n / 2;
    vector<PairedTasks> res { k };

    std::sort(task_durations.begin(), task_durations.end());

    for (size_t i = 0; i < n / 2; ++i)
      res[i].task_1 = task_durations[i];

    for (size_t i = 0; i < n / 2; ++i)
      res[k - 1 - i].task_2 = task_durations[n / 2 + i];

    return res;
  }
}

vector<PairedTasks> OptimumTaskAssignment(vector<int> task_durations) {
  return cache_friendly(task_durations);
}

namespace test_framework {
template <>
struct SerializationTrait<PairedTasks> : UserSerTrait<PairedTasks, int, int> {};
}  // namespace test_framework

bool operator==(const PairedTasks& lhs, const PairedTasks& rhs) {
  return std::tie(lhs.task_1, lhs.task_2) == std::tie(rhs.task_1, rhs.task_2);
}

std::ostream& operator<<(std::ostream& out, const PairedTasks& t) {
  return PrintTo(out, std::make_tuple(t.task_1, t.task_2));
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"task_durations"};
  return GenericTestMain(args, "task_pairing.cc", "task_pairing.tsv",
                         &OptimumTaskAssignment, DefaultComparator{},
                         param_names);
}
