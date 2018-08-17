#include "combinations.h"

namespace sp {
namespace rec {
//=====================================
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
}
}
