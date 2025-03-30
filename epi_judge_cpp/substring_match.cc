#include <string>
#include <string_view>

#include "test_framework/generic_test.h"

#define LOG(x) std::cout << #x << ": " << (x) << "\n";

// Returns the index of the first character of the substd::string if found, -1
// otherwise.

bool substr_equal(const std::string& s, const size_t offset, const std::string& p) {
  return std::string_view{s.data() + offset, p.size()} == std::string_view{p};
}

// Test PASSED (835/835) [ 404 us]
// Average running time:   <1 us
// Median running time:    <1 us
int RabinKarp1(const std::string &t, const std::string &s) {
  if (t.size() < s.size()) {
    return -1;
  }

  constexpr auto p = 132;
  constexpr auto m = 263;
  constexpr auto p_inv_m = 2;

  const auto s_hash = [&s, p, m, p_inv_m]() {
    auto hash = 0;
    for (auto i = 0, p_pow = 1; i < s.size(); ++i, p_pow = (p_pow * p) % m) {
      hash = (hash + (s[i] * p_pow) % m) % m;
    }
    return hash;
  }();

  auto t_window_hash = 0;
  auto p_pow = 1;
  for (auto i = 0; i < s.size(); ++i, p_pow = (p_pow * p) % m) {
    t_window_hash = (t_window_hash + (t[i] * p_pow) % m) % m;
  }
  p_pow = (p_pow * p_inv_m) % m;

  if (t_window_hash == s_hash && substr_equal(t, 0, s)) {
    return 0;
  }

  for (auto i = s.size(); i < t.size(); ++i) {
    t_window_hash = t_window_hash + 2 * m - t[i - s.size()];
    t_window_hash = t_window_hash * p_inv_m;
    t_window_hash = (t_window_hash + (t[i] * p_pow)) % m;

    if (t_window_hash == s_hash && substr_equal(t, i - s.size() + 1, s)) {
      return i - s.size() + 1;
    }
  }

  return -1;
}

// Test PASSED (835/835) [   1 ms]
// Average running time:    1 us
// Median running time:    <1 us
int RabinKarp2(const std::string &t, const std::string &s) {
  if (t.size() < s.size()) {
    return -1;
  }

  constexpr auto p = 257;
  constexpr auto m = 1'000'000'007;
  constexpr auto p_inv_m = 70'038'911;

  const auto s_hash = [&s, p, m, p_inv_m]() {
    auto hash = 0;
    for (auto i = 0, p_pow = 1; i < s.size(); ++i, p_pow = (1LL * p_pow * p) % m) {
      hash = (hash + (1LL * s[i] * p_pow) % m) % m;
    }
    return hash;
  }();

  auto t_window_hash = 0;
  auto p_pow = 1;
  for (auto i = 0; i < s.size(); ++i, p_pow = (1LL * p_pow * p) % m) {
    t_window_hash = (t_window_hash + (1LL * t[i] * p_pow) % m) % m;
  }
  p_pow = (1LL * p_pow * p_inv_m) % m;

  if (t_window_hash == s_hash && substr_equal(t, 0, s)) {
    return 0;
  }

  for (auto i = s.size(); i < t.size(); ++i) {
    t_window_hash = (t_window_hash + m - t[i - s.size()]) % m;
    t_window_hash = (1LL * t_window_hash * p_inv_m) % m;
    t_window_hash = (t_window_hash + (1LL * t[i] * p_pow) % m) % m;

    if (t_window_hash == s_hash && substr_equal(t, i - s.size() + 1, s)) {
      return i - s.size() + 1;
    }
  }

  return -1;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"t", "s"};
  GenericTestMain(args, "substring_match.cc", "substring_match.tsv",
                         &RabinKarp1, DefaultComparator{}, param_names);
  GenericTestMain(args, "substring_match.cc", "substring_match.tsv",
                         &RabinKarp2, DefaultComparator{}, param_names);
  // LOG(RabinKarp("aabaa", "ab"));
}
