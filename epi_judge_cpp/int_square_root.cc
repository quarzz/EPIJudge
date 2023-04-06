#include "test_framework/generic_test.h"

#define log(x) std::cout << #x << ": " << (x) << std::endl

const int max_sqrt = std::sqrt(std::numeric_limits<int>::max());

int SquareRoot(int k) {
  // TODO - you fill in here.
  // Using max_sqrt is about twice faster, than using long long like in the book
  auto l = 0, r = std::min(max_sqrt, k);

  while (l <= r) {
    const auto m = l + (r - l) / 2;

    if (m * m > k)
      r = m - 1;
    else
      l = m + 1;
  }

  return r;
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"k"};
  return GenericTestMain(args, "int_square_root.cc", "int_square_root.tsv",
                         &SquareRoot, DefaultComparator{}, param_names);
}
