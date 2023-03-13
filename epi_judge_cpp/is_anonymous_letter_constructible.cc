#include <string>
#include <unordered_map>

#include "test_framework/generic_test.h"
using std::string;
using std::unordered_map;

namespace {
  unordered_map<char, int> str_to_freqs(const string& str) {
    unordered_map<char, int> freqs;

    for (const auto c: str)
      ++freqs[c];

    return freqs;
  }

  // Passing
  bool a_bit_silly(const string& letter_text, const string& magazine_text) {
    const auto letter_freqs = str_to_freqs(letter_text);
    auto magazine_freqs = str_to_freqs(magazine_text);

    for (const auto freq: letter_freqs)
      if (magazine_freqs[freq.first] < freq.second)
        return false;

    return true;
  }

  // Passing
  bool a_bit_smarter(const string& letter_text, const string& magazine_text) {
    unordered_map<char, int> letter_freqs;

    for (const auto c: letter_text)
      ++letter_freqs[c];

    for (const auto c: magazine_text) {
      auto it = letter_freqs.find(c);

      if (it == letter_freqs.end())
        continue;

      if (it->second == 1)
        letter_freqs.erase(it);
      else
        --it->second;
    }

    return letter_freqs.empty();
  }
}

bool IsLetterConstructibleFromMagazine(
  const string& letter_text,
  const string& magazine_text
) {
  return a_bit_smarter(letter_text, magazine_text);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"letter_text", "magazine_text"};
  return GenericTestMain(args, "is_anonymous_letter_constructible.cc",
                         "is_anonymous_letter_constructible.tsv",
                         &IsLetterConstructibleFromMagazine,
                         DefaultComparator{}, param_names);
}
