#include <algorithm>
#include <iterator>
#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

namespace {
  bool is_all_valid(const vector<int>& queens) {
    for (size_t i = 0; i + 1 < queens.size(); ++i)
      for (size_t j = i + 1; j < queens.size(); ++j)
        if (queens[i] == queens[j] || j - i == std::abs(queens[j] - queens[i]))
          return false;

    return true;
  }

  bool is_last_valid(const vector<int>& queens) {
    const auto last_i = queens.size() - 1;
    const auto last = queens.back();

    for (size_t i = 0; i < queens.size() - 1; ++i)
        if (queens[i] == last || last_i - i == std::abs(last - queens[i]))
          return false;

    return true;
  }

  // 10+ minutes :D
  void super_naive(int n, vector<int>& queens, vector<vector<int>>& result) {
    if (queens.size() == n) {
      if (is_all_valid(queens))
        result.emplace_back(queens);
      return;
    }

    for (int i = 0; i < n; ++i) {
      queens.emplace_back(i);
      super_naive(n, queens, result);
      queens.pop_back();
    }
  }

  // Much better ^_^ 30ms
  void less_naive(int n, vector<int>& queens, vector<vector<int>>& result) {
    if (queens.size() == n) {
      result.emplace_back(queens);
      return;
    }

    for (int i = 0; i < n; ++i) {
      queens.emplace_back(i);
      if (is_last_valid(queens))
        less_naive(n, queens, result);
      queens.pop_back();
    }
  }

  // Now as fast as I wanted it to be.
  // TODO: refactor
  // TODO: check whether it can optimized so that it beats less_naive
  void too_smart(
    int n,
    vector<int>& queens,
    vector<vector<int>>& result,
    std::unordered_set<int>& columns,
    std::unordered_map<int, std::unordered_set<int>>& diags
  ) {
    if (queens.size() == n) {
      result.emplace_back(queens);
      return;
    }

    for (int column = 0; column < n; ++column) {
      const auto i = queens.size();

      if (columns.find(column) == columns.end())
        continue;
      if (diags[i].find(column) != diags[i].end())
        continue;

      std::vector<int> removed_columns;
      for (int j = i + 1; j < n; ++j) {
        int col_ = column + (j - i);
        if (col_ >= 0 && col_ < n) {
          if (diags[j].find(col_) == diags[j].end()) {
            diags[j].insert(col_);
            removed_columns.emplace_back(col_);
          }
        }

        col_ = column - (j - i);
        if (col_ >= 0 && col_ < n) {
          if (diags[j].find(col_) == diags[j].end()) {
            diags[j].insert(col_);
            removed_columns.emplace_back(col_);
          }
        }
      }
      queens.emplace_back(column);
      columns.erase(column);
      too_smart(n, queens, result, columns, diags);
      queens.pop_back();
      columns.insert(column);

      for (const auto col: removed_columns) {
        const auto diff = std::abs(col - column);
        diags[i + diff].erase(col);
      }
    }
  }
}

vector<vector<int>> NQueens(int n) {
  // TODO - you fill in here.
  std::vector<std::vector<int>> result;
  std::vector<int> queens;
  std::unordered_set<int> columns;
  for (int i = 0; i < n; ++i)
    columns.insert(i);

  std::unordered_map<int, std::unordered_set<int>> diags;

  too_smart(n, queens, result, columns, diags);
  // less_naive(n, queens, result);
  return result;
}

int main(int argc, char* argv[]) {
  // NQueens(4);

  // return 0;
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"n"};
  return GenericTestMain(args, "n_queens.cc", "n_queens.tsv", &NQueens,
                         UnorderedComparator{}, param_names);
}
