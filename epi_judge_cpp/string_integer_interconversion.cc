#include <string>

#include "test_framework/generic_test.h"
#include "test_framework/test_failure.h"
using std::string;

#define log(x) std::cout << #x << ": " << (x) << std::endl

namespace {

}

string IntToString(int x) {
  if (!x) return "0";

  string s;
  const auto sign = x < 0 ? -1 : 1;

  for (auto i = x; i; i /= 10)
    s += (i % 10) * sign + '0';

  if (sign < 0) s += '-';
  std::reverse(s.begin(), s.end());

  return s;
}

int StringToInt(const string& s) {
  auto x = 0;
  const auto sign = s[0] == '-' ? -1 : 1;
  const auto shift = std::isdigit(s[0]) ? 0 : 1;

  for (auto i = s.begin() + shift; i != s.end(); ++i)
    x = x * 10 + (*i - '0') * sign;

  return x;
}

void Wrapper(int x, const string& s) {
  if (stoi(IntToString(x)) != x) {
    throw TestFailure("Int to string conversion failed");
  }

  if (StringToInt(s) != x) {
    throw TestFailure("String to int conversion failed");
  }
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"x", "s"};
  return GenericTestMain(args, "string_integer_interconversion.cc",
                         "string_integer_interconversion.tsv", &Wrapper,
                         DefaultComparator{}, param_names);
}
