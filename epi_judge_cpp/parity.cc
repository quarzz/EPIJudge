#include "test_framework/generic_test.h"

#include <type_traits>

namespace {
  // Passing
  constexpr std::uint16_t naive(std::uint64_t x) {
    std::uint16_t nbits = 0;

    for (; x; x >>= 1)
      nbits ^= x & 1;

    return nbits;
  }

  // Passing
  template <typename T>
  constexpr typename std::enable_if<
    std::is_integral<T>::value && std::is_unsigned<T>::value,
    std::uint16_t
  >::type smarter(T x) {
    std::uint16_t nbits = 0;

    for (; x; x = x & (x - 1))
      nbits ^= 1;

    return nbits;
  }

  constexpr std::uint64_t CACHE_SIZE = 65536;
  using Cache = std::array<std::uint16_t, CACHE_SIZE>;

  template <std::size_t... I>
  constexpr Cache create_cache(std::index_sequence<I...>) {
    return std::array<std::uint16_t, CACHE_SIZE> { smarter(I)... };
  }

  constexpr std::array<std::uint16_t, CACHE_SIZE> cache = create_cache(std::make_index_sequence<CACHE_SIZE>{});

  // Passing
  constexpr std::uint16_t cached(std::uint64_t x) {
    return cache[x & 0xffff] ^
      cache[(x >> 16) & 0xffff] ^
      cache[(x >> 32) & 0xffff] ^
      cache[(x >> 48) & 0xffff];
  }

  // Passing
  constexpr std::uint16_t smart(std::uint64_t x) {
    x ^= x >> 32;
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;

    return x & 1;
  }

  // Passing
  constexpr std::uint16_t smartest(std::uint64_t x) {
    x ^= x >> 32;
    x ^= x >> 16;

    return cache[x & 0xffff];
  }
};

short Parity(unsigned long long x) {
  return smartest(x);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"x"};
  return GenericTestMain(args, "parity.cc", "parity.tsv", &Parity,
                         DefaultComparator{}, param_names);
}
