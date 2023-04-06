#include <algorithm>
#include <cmath>
#include <vector>

#include "test_framework/generic_test.h"
#include "test_framework/serialization_traits.h"
#include "test_framework/test_utils.h"
using std::vector;

struct Star {
  bool operator<(const Star& that) const {
    return Distance() < that.Distance();
  }

  double Distance() const { return sqrt(x * x + y * y + z * z); }

  double x, y, z;
};

namespace {
  // Passing
  //   But almost two times slower, than vector. Useless adapter.
  vector<Star> using_priority_queue(
    vector<Star>::const_iterator stars_begin,
    const vector<Star>::const_iterator& stars_end,
    int k
  ) {
    if (stars_end - stars_begin < k)
      return { stars_begin, stars_end };

    // Make heap of first k stars
    std::priority_queue<Star> heap { stars_begin, stars_begin + k };

    // Push others into heap, removing furthest elements each iteration
    for (stars_begin += k; stars_begin != stars_end; ++stars_begin)
      if (*stars_begin < heap.top()) {
        heap.pop();
        heap.emplace(*stars_begin);
      }

    std::vector<Star> res;
    while (!heap.empty()) {
      res.emplace_back(heap.top());
      heap.pop();
    }

    return res;
  }

  // Passing
  vector<Star> using_vector(
    vector<Star>::const_iterator stars_begin,
    const vector<Star>::const_iterator& stars_end,
    int k
  ) {
    if (stars_end - stars_begin < k)
      return { stars_begin, stars_end };

    // Make heap of first k stars
    std::vector<Star> closest_stars { stars_begin, stars_begin + k };
    const auto heap_begin = closest_stars.data();
    const auto heap_end = heap_begin + k;
    std::make_heap(closest_stars.begin(), closest_stars.end());

    // Push others into heap, removing furthest elements each iteration
    for (stars_begin += k; stars_begin != stars_end; ++stars_begin)
      if (*stars_begin < *heap_begin) {
        std::pop_heap(heap_begin, heap_end);
        *(heap_end - 1) = *stars_begin;
        std::push_heap(heap_begin, heap_end);
      }

    return closest_stars;
  }
}

vector<Star> FindClosestKStars(vector<Star>::const_iterator stars_begin,
                               const vector<Star>::const_iterator& stars_end,
                               int k) {
  // TODO - you fill in here.
  return using_priority_queue(stars_begin, stars_end, k);
}

namespace test_framework {
template <>
struct SerializationTrait<Star> : UserSerTrait<Star, double, double, double> {};
}  // namespace test_framework

std::ostream& operator<<(std::ostream& out, const Star& s) {
  return out << s.Distance();
}

bool Comp(const vector<double>& expected, vector<Star> output) {
  if (output.size() != expected.size()) {
    return false;
  }

  std::sort(begin(output), end(output));

  for (int i = 0; i < output.size(); ++i) {
    if (!DefaultComparator()(output[i].Distance(), expected[i])) {
      return false;
    }
  }
  return true;
}

vector<Star> FindClosestKStarsWrapper(const vector<Star>& stars, int k) {
  return FindClosestKStars(cbegin(stars), cend(stars), k);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"stars", "k"};
  return GenericTestMain(args, "k_closest_stars.cc", "k_closest_stars.tsv",
                         &FindClosestKStarsWrapper, &Comp, param_names);
}
