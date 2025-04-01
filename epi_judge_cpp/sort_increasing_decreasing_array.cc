#include <compare>
#include <queue>
#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

namespace {
struct Range {
  std::vector<int>::iterator first;
  std::vector<int>::iterator last;
};

auto operator<=>(const Range& lhs, const Range& rhs) {
  return *lhs.first <=> *rhs.first;
}

enum class Direction {Down = -1, Up = 1};

Direction operator-(const Direction direction) {
  return static_cast<Direction>(-static_cast<int>(direction));
}

vector<int> SortKIncreasingDecreasingArray(vector<int> A) {
  if (A.size() < 2) return A;

  const auto size = A.size();
  auto direction = A[0] <= A[1] ? Direction::Up : Direction::Down;
  std::priority_queue<Range, std::vector<Range>, std::greater<Range>> heap;

  size_t prev_peak = 0;
  for (size_t i = 1; i < size - 1; ++i) {
    if (direction == Direction::Up && A[i] <= A[i + 1]) continue;
    if (direction == Direction::Down && A[i] >= A[i + 1]) continue;

    if (direction == Direction::Down)
      std::reverse(A.begin() + prev_peak, A.begin() + i);
    heap.push({A.begin() + prev_peak, A.begin() + i});

    prev_peak = i;
    direction = -direction;
  }
  if (direction == Direction::Down)
    std::reverse(A.begin() + prev_peak, A.end());
  heap.push({A.begin() + prev_peak, A.end()});

  std::vector<int> result;
  result.reserve(size);
  while (!heap.empty()) {
    const auto [first, last] = heap.top();
    heap.pop();
    result.push_back(*first);
    if (first + 1 != last)
      heap.push({first + 1, last});
  }

  return result;
}
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"A"};
  return GenericTestMain(args, "sort_increasing_decreasing_array.cc",
                         "sort_increasing_decreasing_array.tsv",
                         &SortKIncreasingDecreasingArray, DefaultComparator{},
                         param_names);
}
