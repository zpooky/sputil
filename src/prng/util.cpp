#include "util.h"

#include <algorithm>
#include <prng/URandom.h>
#include <prng/xorshift.h>

namespace prng {
//=====================================
template <typename Random>
typename Random::Word
uniform_dist(Random &r, typename Random::Word inc,
             typename Random::Word ex) noexcept {
  assertxs(inc >= 0, inc);
  assertxs(inc < ex, inc, ex);

  typename Random::Word res{random(r)};
  return (res % (ex - inc)) + inc;
}

/* explicit template instantiation */
template typename xorshift32::Word
uniform_dist(xorshift32 &, typename xorshift32::Word,
             typename xorshift32::Word) noexcept;

template typename xorshift128plus::Word
uniform_dist(xorshift128plus &, typename xorshift128plus::Word,
             typename xorshift128plus::Word) noexcept;

//=====================================
template <typename Random>
bool
uniform_bool(Random &r) noexcept {
  auto res = uniform_dist(r, /*>=*/0, /*<*/ 2);
  return res == 1 ? true : false;
}

/* explicit template instantiation */
template bool
uniform_bool(xorshift32 &r) noexcept;

template bool
uniform_bool(xorshift128plus &r) noexcept;

//=====================================
template <typename Random>
void
fill(Random &r, void *buffer, std::size_t size, dist_f<Random> dist) noexcept {
  std::size_t idx = 0;
  unsigned char *it = (unsigned char *)buffer;

  while (idx < size) {
    auto stuff = dist(r);

    if (stuff != 0) { // TODO?
      const std::size_t len = std::min(size - idx, sizeof(stuff));
      std::memcpy(it + idx, &stuff, len);
      idx += len;
    }
  }
}

/* explicit template instantiation */
template void
fill(xorshift32 &, void *, std::size_t, dist_f<xorshift32>) noexcept;

template void
fill(xorshift128plus &, void *, std::size_t, dist_f<xorshift128plus>) noexcept;

//=====================================
template <typename Random>
void
fill(Random &r, void *buffer, std::size_t size) noexcept {
  std::size_t idx = 0;
  unsigned char *it = (unsigned char *)buffer;

  while (idx < size) {
    assertx(it);

    auto stuff = random(r);

    if (stuff != 0) { // TODO?
      const std::size_t len = std::min(size - idx, sizeof(stuff));
      std::memcpy(it + idx, &stuff, len);
      idx += len;
    }
  }
}

/* explicit template instantiation */
template void
fill(xorshift32 &, void *, std::size_t) noexcept;

template void
fill(xorshift128plus &, void *, std::size_t) noexcept;

//=====================================
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

//=====================================
} // namespace prng
