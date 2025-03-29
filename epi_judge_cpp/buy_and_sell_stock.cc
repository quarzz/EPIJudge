#include <set>
#include <vector>

#include "test_framework/generic_test.h"

using std::multiset;
using std::vector;

#define LOG(x) std::cout << #x << ": " << (x) << "\n"

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
    // Invariants:
    // best_profit on prefix before current price
    double best_profit = 0.0;
    // min_price on prefix before current price
    double min_price = std::numeric_limits<double>::max();

    for (const auto price: prices) {
      best_profit = std::max(price - min_price, best_profit);
      min_price = std::min(price, min_price);
    }

    return best_profit;
  }

  int longest_equal_subarray(const std::vector<int>& nums) {
    // Invariants:
    // first – leftmost start of equal subarray with elements equal to nums[i - 1]
    // max_length – max length of such subarrays so far
    size_t first = 0;
    size_t max_length = 1;
    for (size_t i = 1; i < nums.size(); ++i) {
      if (nums[i] != nums[i - 1]) {
        max_length = std::max(max_length, i - first);
        first = i;
      }
    }
    max_length = std::max(max_length, nums.size() - first);
    return max_length;
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
