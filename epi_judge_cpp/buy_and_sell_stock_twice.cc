#include <limits>
#include <span>
#include <type_traits>
#include <vector>

#include "test_framework/generic_test.h"
using std::vector;

#define LOG(x) std::cout << #x << ": " << (x) << "\n"

namespace {
  double BuyAndSellStockOne(const std::span<const double>& prices) {
    // Invariants:
    // best_profit on prefix before current price
    double max_profit = 0.0;
    // min_price on prefix before current price
    double min_price = std::numeric_limits<double>::max();

    for (const auto price: prices) {
      max_profit = std::max(price - min_price, max_profit);
      min_price = std::min(price, min_price);
    }

    return max_profit;
  }

  // With -O2
  // Test PASSED (402/402) [   2  s]
  // Average running time:   14 ms
  // Median running time:     2 us
  //
  // Without -O2
  // Test PASSED (402/402) [  23  s]
  // Average running time:  117 ms
  // Median running time:    31 us
  double naively_reuse_solution_for_once(const vector<double>& prices) {
    double max_profit = 0;

    for (size_t i = 0; i < prices.size(); ++i) {
      const std::span left{prices.begin(), prices.begin() + i + 1};
      const std::span right{prices.begin() + i, prices.end()};
      const auto profit = BuyAndSellStockOne(left) + BuyAndSellStockOne(right);
      max_profit = std::max(max_profit, profit);
    }

    return max_profit;
  }

  // With -O2
  // Test PASSED (402/402) [ 286 us]
  // Average running time:    1 us
  // Median running time:    <1 us
  //
  // Without -O2
  // Test PASSED (402/402) [   2 ms]
  // Average running time:   16 us
  // Median running time:     3 us
  double using_preprocessing(const std::span<const double>& prices) {
    // pre[i] – best BuyAndSellStockOne on prefix [0, i]
    const auto pre = [&prices]() {
      vector pre(prices.size(), 0.0);
      auto max_profit = 0.0, min_price = std::numeric_limits<double>::max();
      for (size_t i = 0; i < prices.size(); ++i) {
        max_profit = std::max(max_profit, prices[i] - min_price);
        pre[i] = max_profit;
        min_price = std::min(min_price, prices[i]);
      }
      return pre;
    }();

    // post[i] – best BuyAndSellStockOne on postfix [i, n - 1]
    const auto post = [&prices]() {
      vector post(prices.size() + 1, 0.0);
      auto max_profit = 0.0, max_price = std::numeric_limits<double>::min();
      for (size_t i = prices.size(); i--;) {
        max_profit = std::max(max_profit, max_price - prices[i]);
        post[i] = max_profit;
        max_price = std::max(max_price, prices[i]);
      }
      return post;
    }();

    auto max_profit = 0.0;
    for (size_t i = 0; i < prices.size(); ++i) {
      max_profit = std::max(max_profit, pre[i] + post[i + 1]);
    }
    return max_profit;
  }

  // With -O2
  // Test PASSED (402/402) [ 209 us]
  // Average running time:    1 us
  // Median running time:    <1 us
  //
  // Without -O2
  // Test PASSED (402/402) [   1 ms]
  // Average running time:   10 us
  // Median running time:     2 us
  double preprocessing_pre(const std::span<const double>& prices) {
    // pre[i] – best BuyAndSellStockOne on prefix [0, i]
    const auto pre = [&prices]() {
      vector pre(prices.size(), 0.0);
      auto max_profit = 0.0, min_price = std::numeric_limits<double>::max();
      for (size_t i = 0; i < prices.size(); ++i) {
        max_profit = std::max(max_profit, prices[i] - min_price);
        pre[i] = max_profit;
        min_price = std::min(min_price, prices[i]);
      }
      return pre;
    }();

    // Invariants for [i + 1, n - 1]
    auto max_post_price = std::numeric_limits<double>::min();
    auto max_post_profit = 0.0;
    auto max_total_profit = 0.0;
    for (size_t i = prices.size(); i--;) {
      max_total_profit = std::max(max_total_profit, pre[i] + max_post_profit);

      max_post_profit = std::max(max_post_profit, max_post_price - prices[i]);
      max_post_price = std::max(max_post_price, prices[i]);
    }

    return max_total_profit;
  }

  // With -O2
  // Test PASSED (402/402) [ 178 us]
  // Average running time:    1 us
  // Median running time:    <1 us
  //
  // Without -O2
  // Test PASSED (402/402) [   1 ms]
  // Average running time:   11 us
  // Median running time:     2 us
  double preprocessing_post(const std::span<const double>& prices) {
    // post[i] – best BuyAndSellStockOne on postfix [i, n - 1]
    const auto post = [&prices]() {
      vector post(prices.size() + 1, 0.0);
      auto max_profit = 0.0, max_price = std::numeric_limits<double>::min();
      for (size_t i = prices.size(); i--;) {
        max_profit = std::max(max_profit, max_price - prices[i]);
        post[i] = max_profit;
        max_price = std::max(max_price, prices[i]);
      }
      return post;
    }();

    // Invariants for prefix [0, i]
    auto min_pre_price = std::numeric_limits<double>::max();
    auto max_pre_profit = 0.0;
    auto max_total_profit = 0.0;
    for (size_t i = 0; i < prices.size(); ++i) {
      max_pre_profit = std::max(max_pre_profit, prices[i] - min_pre_price);
      max_total_profit = std::max(max_total_profit, max_pre_profit + post[i + 1]);
      min_pre_price = std::min(min_pre_price, prices[i]);
    }

    return max_total_profit;
  }

  template<typename T>
  std::ostream& operator<<(std::ostream& out, const vector<T>& v) {
    for (const auto& el : v) {
      out << el << ' ';
    }
    return out;
  }

  double using_dp(const std::span<const double>& prices) {
    auto max_profit = 0.0;
    std::array<double, 5> dp{};
    for (size_t i = prices.size(); i--;) {
      dp[0] = std::max(dp[1] - prices[i], dp[0]);
      dp[1] = std::max(dp[2] + prices[i], dp[1]);
      dp[2] = std::max(dp[3] - prices[i], dp[2]);
      dp[3] = std::max(prices[i], dp[3]);
      max_profit = std::max(max_profit, dp[0]);
    }

    return max_profit;
  }

  template<typename T>
  requires std::is_arithmetic_v<T>
  double using_dp(const std::span<const T>& prices, size_t max_buys) {
    T max_profit{};
    std::vector<T> dp(max_buys * 2 + 2);
    for (size_t i = prices.size(); i--;) {
      for (size_t j = 0; j < max_buys; ++j) {
        dp[2 * j] = std::max(dp[2 * j + 1] - prices[i], dp[2 * j]);
        dp[2 * j + 1] = std::max(dp[2 * j + 2] + prices[i], dp[2 * j + 1]);
      }
      max_profit = std::max(max_profit, dp[0]);
    }

    return max_profit;
  }
}

double BuyAndSellStockTwice(const vector<double>& prices) {
  return using_dp(std::span<const double>{prices}, 2);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"prices"};
  return GenericTestMain(args, "buy_and_sell_stock_twice.cc",
                         "buy_and_sell_stock_twice.tsv", &BuyAndSellStockTwice,
                         DefaultComparator{}, param_names);
}
