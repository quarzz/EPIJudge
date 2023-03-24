#include "test_framework/generic_test.h"

namespace {
  double naive_positive(double x, int y);

  // Passing
  //   I guess this one is more precise, because of denormalized doubles
  //   But because of these denormals it can be slower for negative powers
  double naive_with_denorms(double x, int y) {
    return y < 0 ? naive_positive(1.0 / x, -y) : naive_positive(x, y);
  }

  // Passing
  //   This one, for example, is a bit faster
  //   But I am not sure whether it is as precise. Need to check later.
  // TODO: read the line above
  double naive_without_denorms(double x, int y) {
    return y < 0 ? 1.0 / naive_positive(x, -y) : naive_positive(x, y);
  }

  // Passing
  // Slowest
  double naive_iffed(double x, int y) {
    double res = 1.0;

    while (y) {
      if (y > 0) {
        res *= x;
        --y;
      } else {
        res /= x;
        ++y;
      }
    }

    return res;
  }

  double naive_positive(double x, int y) {
    double res = 1.0;
    while (y--) res *= x;
    return res;
  }

  double log_positive(double x, int y);

  // Passing
  double log_with_denorms(double x, int y) {
    return y < 0 ? log_positive(1.0 / x, -y) : log_positive(x, y);
  }

  double ad_hoc(double x, int y) {
    return y < 0 ? log_positive(1 / x, -y) : log_positive(x, y);
  }

  // Passing
  double log_without_denorms(double x, int y) {
    return y < 0 ? 1.0 / log_positive(x, -y) : log_positive(x, y);
  }

  double log_positive(double x, int y) {
    // Invariant: res = s * x ^ y
    auto s = 1.0;

    while (y) {
      if (y % 2) {
        s *= x;
        --y;
      } else {
        x *= x;
        y /= 2;
      }
    }

    return s;
  }

  // P.S. The difference in denorms/no denorms vanished after changing / to *
}

double Power(double x, int y) {
  // TODO - you fill in here.
  return log_with_denorms(x, y);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"x", "y"};
  return GenericTestMain(args, "power_x_y.cc", "power_x_y.tsv", &Power,
                         DefaultComparator{}, param_names);
}
