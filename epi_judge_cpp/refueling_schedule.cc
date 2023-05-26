#include <vector>

#include "test_framework/fmt_print.h"
#include "test_framework/generic_test.h"
#include "test_framework/test_failure.h"
#include "test_framework/timed_executor.h"
using std::vector;
const int kMPG = 20;

namespace {
  int adhoc(const std::vector<int>& gallons, const std::vector<int>& distances) {
    const auto ncities = gallons.size();
    auto gas_left = 0;
    auto last_city = -1;

    for (size_t i = 0; i < ncities; ++i) {
      gas_left += gallons[i] * kMPG - distances[i];
      if (gas_left < 0) {
        last_city = i;
        gas_left = 0;
      }
    }

    return last_city + 1;
  }
}

// gallons[i] is the amount of gas in city i, and distances[i] is the distance
// city i to the next city.
int FindAmpleCity(const vector<int>& gallons, const vector<int>& distances) {
  // TODO - you fill in here.
  return adhoc(gallons, distances);
}
void FindAmpleCityWrapper(TimedExecutor& executor, const vector<int>& gallons,
                          const vector<int>& distances) {
  int result = executor.Run([&] { return FindAmpleCity(gallons, distances); });
  const int num_cities = gallons.size();
  int tank = 0;
  for (int i = 0; i < num_cities; ++i) {
    int city = (result + i) % num_cities;
    tank += gallons[city] * kMPG - distances[city];
    if (tank < 0) {
      throw TestFailure(FmtStr("Out of gas on city {}", i));
    }
  }
}

int main(int argc, char* argv[]) {
  // const std::vector<int> gallons {}

  // return 0;

  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"executor", "gallons", "distances"};
  return GenericTestMain(args, "refueling_schedule.cc",
                         "refueling_schedule.tsv", &FindAmpleCityWrapper,
                         DefaultComparator{}, param_names);
}
