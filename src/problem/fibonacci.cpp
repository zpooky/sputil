#include "fibonacci.h"
#include <collection/Array.h>

namespace sp {
std::uint32_t
fibonacci(std::uint32_t in) noexcept {
  if (in <= 1) {
    return in;
  }

  return fibonacci(in - 1) + fibonacci(in - 2);
}

namespace dp {
static std::uint32_t
fibonacci(DynamicArray<std::uint32_t> &memo, std::uint32_t in) noexcept {
  if (in <= 1) {
    return memo[in] = in;
  }

  if (memo[in] == 0) {
    memo[in] = fibonacci(in - 1) + fibonacci(in - 2);
  }

  return memo[in];
}

std::uint32_t
fibonacci(std::uint32_t in) noexcept {
  DynamicArray<std::uint32_t> memo(in + 1);
  memo.length = memo.capacity;
  return fibonacci(memo, in);
}
}
}
