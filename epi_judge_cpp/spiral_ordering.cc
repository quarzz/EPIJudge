#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

namespace {
  // Passing
  vector<int> layer_by_layer(const vector<vector<int>>& A) {
    const size_t n = A.size();
    vector<int> res;
    res.reserve(n * n);

    for (size_t i = 0; i < n / 2; ++i) {
      for (size_t j = i; j < n - 1 - i; ++j)
        res.push_back(A[i][j]);
      for (size_t j = i; j < n - 1 - i; ++j)
        res.push_back(A[j][n - 1 - i]);
      for (size_t j = i; j < n - 1 - i; ++j)
        res.push_back(A[n - 1 - i][n - 1 - j]);
      for (size_t j = i; j < n - 1 - i; ++j)
        res.push_back(A[n - 1 - j][i]);
    }

    if (n % 2)
      res.push_back(A[n / 2][n / 2]);

    return res;
  }
}

vector<int> MatrixInSpiralOrder(const vector<vector<int>>& square_matrix) {
  return layer_by_layer(square_matrix);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"square_matrix"};
  return GenericTestMain(args, "spiral_ordering.cc", "spiral_ordering.tsv",
                         &MatrixInSpiralOrder, DefaultComparator{},
                         param_names);
}
