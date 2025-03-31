#include <cctype>
#include <string>

#include "test_framework/generic_test.h"
using std::string;

#define LOG(x) std::cout << #x << ": " << (x) << "\n"

int Evaluate(const std::string& expression) {
  std::stack<int> stack;

  for (size_t i = 0; i < expression.size(); ++i) {
    const auto c = expression[i];
    if (std::isdigit(c) || (i + 1 < expression.size() && expression[i + 1] != ',')) {
      int number = 0;
      const auto sign = c == '-' ? -1 : 1;
      if (c == '-') ++i;
      while (i < expression.size() && expression[i] != ',') {
        number = number * 10 + (expression[i++] - '0') * sign;
      }
      stack.push(number);
      continue;
    }
    const auto term2 = stack.top();
    stack.pop();
    const auto term1 = stack.top();
    stack.pop();
    switch (c) {
    case '+': stack.push(term1 + term2); break;
    case '-': stack.push(term1 - term2); break;
    case '*': stack.push(term1 * term2); break;
    case '/': stack.push(term1 / term2); break;
    }
    ++i;
  }

  return stack.top();
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"expression"};
  return GenericTestMain(args, "evaluate_rpn.cc", "evaluate_rpn.tsv", &Evaluate,
                         DefaultComparator{}, param_names);
}
