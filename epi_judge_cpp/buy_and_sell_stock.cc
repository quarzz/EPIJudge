#include <set>
#include <vector>

#include "test_framework/generic_test.h"

using std::multiset;
using std::vector;

namespace {
  double brute_force(const vector<double>& prices) {
    double res = 0.0;

    for (size_t i = 0; i < prices.size(); ++i)
      for (size_t j = i + 1; j < prices.size(); ++j)
        res = std::max(res, prices[j] - prices[i]);

    return res;
  }

  double tree(const vector<double>& prices) {
    double res = 0.0;
    multiset<double> tree { prices.begin(), prices.end() };

    for (size_t i = 0; i + 1 < prices.size(); ++i) {
      tree.erase(tree.find(prices[i]));
      const auto max = *std::prev(tree.end());
      res = std::max(max - prices[i], res);
    }

    return res;
  }

  double moving_min(const vector<double>& prices) {
    double res = 0.0;
    double min = std::numeric_limits<double>::max();

    for (const auto price: prices) {
      res = std::max(price - min, res);
      min = std::min(price, min);
    }

    return res;
  }
}

double BuyAndSellStockOnce(const vector<double>& prices) {
  return moving_min(prices);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"prices"};
  return GenericTestMain(args, "buy_and_sell_stock.cc",
                         "buy_and_sell_stock.tsv", &BuyAndSellStockOnce,
                         DefaultComparator{}, param_names);
}
