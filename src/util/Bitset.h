#ifndef SP_UTIL_COLLECTION_BITSET_H
#define SP_UTIL_COLLECTION_BITSET_H

#include <cstddef>
#include <cstdint>

namespace sp {
struct Bitset {
  std::uint64_t *buffer;
  const std::size_t capacity;

  Bitset(std::uint64_t *, std::size_t) noexcept;

  template <std::size_t SIZE>
  explicit Bitset(std::uint64_t (&)[SIZE]) noexcept;
};

template <std::size_t cap>
struct StaticBitset : public Bitset {
  std::uint64_t raw[cap];

  StaticBitset() noexcept;
};

/*
 * capacity: block_size * blocks;
 * Block are lazy allocated when required.
 * Blocks are placed in a BST.
 */
struct SparseBitset {
  const std::size_t block_size;
  const std::size_t blocks;
  void *tree;

  SparseBitset(std::size_t, std::size_t) noexcept;

  ~SparseBitset() noexcept;
};

/*
 * returns the value
 */
bool
test(const Bitset &, std::size_t) noexcept;

bool
test(const SparseBitset &, std::size_t) noexcept;

/*
 * returns the old value
 */
bool
set(Bitset &, std::size_t, bool) noexcept;

bool
set(SparseBitset &, std::size_t, bool) noexcept;

/*
 * returns the new value
 */
bool
toggle(Bitset &, std::size_t) noexcept;

bool
toggle(SparseBitset &, std::size_t) noexcept;

/*
 * return the bit capacity of the Bitset
 */
std::size_t
bits(const Bitset &) noexcept;

std::size_t
bits(const SparseBitset &) noexcept;

/*
 * ==========================================================================
 */
template <std::size_t SIZE>
Bitset::Bitset(std::uint64_t (&b)[SIZE]) noexcept
    : buffer(b)
    , capacity(SIZE) {
  // for (std::size_t i = 0; i < capacity; ++i) {
  //   assert(buffer[i] == 0);
  // }
}

template <std::size_t c>
StaticBitset<c>::StaticBitset() noexcept
    : Bitset(raw)
    , raw{0} {
}

} // namespace sp

#endif
