#include "util.h"

#include <algorithm>
#include <prng/URandom.h>
#include <prng/xorshift.h>

namespace prng {

template <typename Random>
void
fill(Random &r, void *buffer, std::size_t size) noexcept {
  std::size_t idx = 0;
  unsigned char *it = (unsigned char *)buffer;
  while (idx < size) {
    auto stuff = random(r);
    if (stuff != 0) {
      const std::size_t len = std::min(size - idx, sizeof(stuff));
      std::memcpy(it + idx, &stuff, len);
      idx += len;
    }
  }
}

/*
 * explicit template instantiation
 */
template void
fill(xorshift32 &, void *, std::size_t) noexcept;

template void
fill(xorshift128plus &, void *, std::size_t) noexcept;

template <typename Random>
Random
seed() noexcept {
  typename Random::InitType init{};
  URandom r;
  fill(r, &init, sizeof(init));

  return Random{init};
}

/*
 * explicit template instantiation
 */
template xorshift32
seed() noexcept;
template xorshift128plus
seed() noexcept;

} // namespace prng
