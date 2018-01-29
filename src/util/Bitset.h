#ifndef SP_UTIL_COLLECTION_BITSET_H
#define SP_UTIL_COLLECTION_BITSET_H

#include <cstdint>
#include <cstddef>

namespace sp {
//TODO SparseBitset
struct Bitset {
  std::uint64_t *buffer;
  const std::size_t capacity;

  Bitset(std::uint64_t *,std::size_t )noexcept;

  template<std::size_t SIZE>
  Bitset(std::uint64_t (&)[SIZE])noexcept;
};

/*
 * returns the value
 */
bool test(const Bitset&, std::size_t) noexcept;

/*
 * returns the old value
 */
bool set(Bitset&, std::size_t, bool) noexcept;

/*
 * returns the new value
 */
bool toggle(Bitset&, std::size_t) noexcept;

/*
 * return the bit capacity of the Bitset
 */
std::size_t bits(const Bitset&) noexcept;


/*
 * ==========================================================================
 */
template<std::size_t SIZE>
Bitset::Bitset(std::uint64_t (&b)[SIZE])noexcept 
  : buffer(b), capacity(SIZE) {
}

}//namespace sp


#endif
