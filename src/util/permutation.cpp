#include "permutation.h"
#include <cmath>

namespace sp {
namespace rec {

std::size_t
permutations_of(std::size_t pool, std::size_t depth) noexcept {
  return std::pow(pool, depth);
}

std::size_t
combinations_of(std::size_t length) noexcept {
  // factorial
  if (length == 0) {
    return 0;
  }

  if (length == 1) {
    return 1;
  }

  return length * combinations_of(length - 1);
}

} // namespace rec
} // namespace sp
