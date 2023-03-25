#include <string>
#include <vector>

#include "test_framework/generic_test.h"
using std::string;
using std::vector;

#define log(x) std::cout << #x << ": " << (x) << std::endl

namespace {
  inline vector<int> init_c_to_i() {
    vector<int> res(256);
    for (auto i = '0'; i <= '9'; ++i)
      res[i] = i - '0';

    for (auto i = 'A'; i <= 'F'; ++i)
      res[i] = i - 'A' + 10;

    return res;
  }

  inline vector<char> init_i_to_c() {
    vector<char> res(16);
    for (size_t i = 0; i < 10; ++i)
      res[i] = i + '0';
    for (size_t i = 10; i < 16; ++i)
      res[i] = i - 10 + 'A';

    return res;
  }

  const vector<int> c_to_i = init_c_to_i();
  const vector<char> i_to_c = init_i_to_c();

  int from_base(const string& s, int b);
  string to_base(int x, int base);

  inline string with_intermediary(const string& s, int b1, int b2) {
    return to_base(from_base(s, b1), b2);
  }

  inline int from_base(const string& s, int b) {
    const auto sign = s[0] == '-' ? -1LL : 1LL;
    const auto from = s[0] == '-' || s[0] == '+' ? s.begin() + 1 : s.begin();

    return sign * std::accumulate(from, s.cend(), 0LL, [b, sign](auto x, auto c) {
        return x * b + c_to_i[c];
    });
  }

  inline string to_base(int x, int base) {
    string res;
    const auto sign = x >= 0 ? 1 : -1;
    x = std::abs(x);

    do {
      res.push_back(i_to_c[x % base]);
      x /= base;
    } while (x);

    if (sign < 0) res.push_back('-');
    std::reverse(res.begin(), res.end());

    return res;
  }
}

inline string ConvertBase(const string& num_as_string, int b1, int b2) {
  return with_intermediary(num_as_string, b1, b2);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"num_as_string", "b1", "b2"};

  const auto start = std::chrono::steady_clock::now();

  for (int i = 0; i < 1000000; ++i)
    ConvertBase("-123456789", 12, 16);

  const auto finish = std::chrono::steady_clock::now();
  const auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
    finish - start
  ).count();
  // All the microoptimizations won only 8% performance
  log(dur);

  return GenericTestMain(args, "convert_base.cc", "convert_base.tsv",
                         &ConvertBase, DefaultComparator{}, param_names);
}
