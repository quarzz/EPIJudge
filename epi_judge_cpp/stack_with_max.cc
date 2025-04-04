#include <stack>
#include <stdexcept>

#include "test_framework/generic_test.h"
#include "test_framework/serialization_traits.h"
#include "test_framework/test_failure.h"

using std::length_error;
using std::stack;

class UsingStackWithMaxes {
public:
  bool Empty() const {
    return s.empty();
  }
  int Max() const {
    return s_max.top();
  }
  int Pop() {
    int val = s.top();
    s.pop();
    s_max.pop();
    return val;
  }
  void Push(int x) {
    s.push(x);
    s_max.push(s_max.empty() ? x : std::max(s_max.top(), x));
  }

private:
  std::stack<int> s;
  std::stack<int> s_max;
};

class UsingReducedStackWithMaxes {
  struct ReducedMax {
    int max;
    int count;
  };
public:
  bool Empty() const {
    return s.empty();
  }
  int Max() const {
    return s_max.top().max;
  }
  int Pop() {
    int val = s.top();
    s.pop();

    if (s_max.top().count == 1)
      s_max.pop();
    else
      --s_max.top().count;

    return val;
  }
  void Push(int x) {
    s.push(x);

    if (Empty() || x > Max())
      s_max.push({ x, 1 });
    else
      ++s_max.top().count;
  }

private:
  std::stack<int> s;
  std::stack<ReducedMax> s_max;
};

struct StackOp {
  std::string op;
  int argument;
};

namespace test_framework {
template <>
struct SerializationTrait<StackOp> : UserSerTrait<StackOp, std::string, int> {};
}  // namespace test_framework

using Stack = UsingStackWithMaxes;

void StackTester(const std::vector<StackOp>& ops) {
  try {
    Stack s;
    for (auto& x : ops) {
      if (x.op == "Stack") {
        continue;
      } else if (x.op == "push") {
        s.Push(x.argument);
      } else if (x.op == "pop") {
        int result = s.Pop();
        if (result != x.argument) {
          throw TestFailure("Pop: expected " + std::to_string(x.argument) +
                            ", got " + std::to_string(result));
        }
      } else if (x.op == "max") {
        int result = s.Max();
        if (result != x.argument) {
          throw TestFailure("Max: expected " + std::to_string(x.argument) +
                            ", got " + std::to_string(result));
        }
      } else if (x.op == "empty") {
        int result = s.Empty();
        if (result != x.argument) {
          throw TestFailure("Empty: expected " + std::to_string(x.argument) +
                            ", got " + std::to_string(result));
        }
      } else {
        throw std::runtime_error("Unsupported stack operation: " + x.op);
      }
    }
  } catch (length_error&) {
    throw TestFailure("Unexpected length_error exception");
  }
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"ops"};
  return GenericTestMain(args, "stack_with_max.cc", "stack_with_max.tsv",
                         &StackTester, DefaultComparator{}, param_names);
}
