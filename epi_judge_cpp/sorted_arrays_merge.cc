#include <queue>
#include <vector>

#include "test_framework/generic_test.h"
using std::vector;
using std::priority_queue;

namespace {
  struct HeapData {
    vector<int>::const_iterator i;
    vector<int>::const_iterator to;
  };

  bool operator>(const HeapData& l, const HeapData& r) {
    return *l.i > *r.i;
  }
}

// It works two times faster if used with hand-implemented heap
// But I guess the cost is that this faster implementation is less production-ready
vector<int> MergeSortedArrays(const vector<vector<int>>& sorted_arrays) {
  vector<int> res;
  priority_queue<HeapData, vector<HeapData>, std::greater<HeapData>> heap;

  for (size_t i = 0; i < sorted_arrays.size(); ++i) {
    if (sorted_arrays[i].empty()) continue;

    heap.push({ sorted_arrays[i].begin(), sorted_arrays[i].end() - 1});
  }

  while (!heap.empty()) {
    const auto top = heap.top();
    res.emplace_back(*top.i);
    heap.pop();

    if (top.i != top.to)
      heap.push({ top.i + 1, top.to });
  }

  return res;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"sorted_arrays"};
  return GenericTestMain(args, "sorted_arrays_merge.cc",
                         "sorted_arrays_merge.tsv", &MergeSortedArrays,
                         DefaultComparator{}, param_names);
}
