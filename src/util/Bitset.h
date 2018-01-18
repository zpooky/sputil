#ifndef SP_UTIL_COLLECTION_BITSET_H
#define SP_UTIL_COLLECTION_BITSET_H

#include <cstdint>
#include <cstddef>

namespace sp {
//   template<typename T,std::size_t SIZE >
// struct StaticBiset {
//   T[SIZE] buffer;
//   Bitset() noexcept  : buffer{0} {
//   }
// };

struct Bitset {
  std::uint64_t *buffer;
  const std::size_t capacity;

  Bitset(std::uint64_t *,std::size_t )noexcept;
};

//TODO SparseBitset

bool test(const Bitset&, std::size_t) noexcept;
bool set(Bitset&, std::size_t, bool) noexcept;
bool toggle(Bitset&, std::size_t) noexcept;


}//namespace sp


#endif
