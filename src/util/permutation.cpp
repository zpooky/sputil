#include "permutation.h"
#include <cmath>

namespace sp {
namespace rec {
//=====================================
std::size_t
permutations_of(std::size_t pool, std::size_t depth) noexcept {
  return std::pow(pool, depth);
}


//=====================================
} // namespace rec
} // namespace sp
