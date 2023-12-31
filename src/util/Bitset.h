#ifndef SP_UTIL_UTIL_BITSET_H
#define SP_UTIL_UTIL_BITSET_H

#include <cstddef>
#include <cstdint>
#include <util/assert.h>

namespace sp {
//=====================================
struct Bitset {
  std::uint64_t *buffer;
  std::size_t capacity;

  Bitset(std::uint64_t *, std::size_t) noexcept;

  template <std::size_t SIZE>
  explicit Bitset(std::uint64_t (&)[SIZE]) noexcept;

  virtual ~Bitset() noexcept {
  }
};

//=====================================
template <std::size_t cap>
struct StaticBitset : public Bitset {
  std::uint64_t raw[cap];

  StaticBitset() noexcept;
};

//=====================================
struct DynamicBitset : public Bitset {
  explicit DynamicBitset(std::size_t number_of_u64_buffers) noexcept;

  ~DynamicBitset() noexcept;
};

//=====================================
/* capacity: block_size * blocks;
 * Block are lazy allocated when required.
 * Blocks are placed in a BST.
 */
struct SparseBitset {
  const std::size_t block_size;
  const std::size_t blocks;
  void *tree;

  SparseBitset(std::size_t block_sz, std::size_t blocks) noexcept;

  ~SparseBitset() noexcept;
};

//=====================================
/* returns the current state
 */
bool
test(const Bitset &, std::size_t) noexcept;

bool
test(const SparseBitset &, std::size_t) noexcept;

//=====================================
/* returns the old state
 */
bool
set(Bitset &, std::size_t, bool) noexcept;

bool
set(SparseBitset &, std::size_t, bool) noexcept;

//=====================================
/* returns the new state
 */
bool
toggle(Bitset &, std::size_t) noexcept;

bool
toggle(SparseBitset &, std::size_t) noexcept;

//=====================================
/* return the bit capacity of the Bitset
 */
std::size_t
capacity(const Bitset &) noexcept;

std::size_t
capacity(const SparseBitset &) noexcept;

//=====================================
/* returns number of uint64_t required to fit $x distinct values
 */
std::size_t
Bitset_number_of_buffer(std::size_t x) noexcept;

//=====================================
template <typename F>
void
for_each(const Bitset &, F) noexcept;

template <typename F>
void
for_each(const SparseBitset &, F) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <std::size_t SIZE>
Bitset::Bitset(std::uint64_t (&b)[SIZE]) noexcept
    : buffer(b)
    , capacity(SIZE) {
  // for (std::size_t i = 0; i < capacity; ++i) {
  //   assert(buffer[i] == 0);
  // }
}

//=====================================
template <std::size_t c>
StaticBitset<c>::StaticBitset() noexcept
    : Bitset(raw)
    , raw{0} {
}

//=====================================
template <typename F>
void
for_each(const Bitset &self, F f) noexcept {
  for (std::size_t idx = 0; idx < capacity(self); ++idx) {
    bool v = test(self, idx);
    f(idx, v);
  }
}

template <typename F>
void
for_each(const SparseBitset &, F) noexcept {
  assertx(false);
  // TODO
}

//=====================================
} // namespace sp

#endif
