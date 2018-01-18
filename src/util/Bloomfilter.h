#ifndef SP_UTIL_COLLECTION_BLOOM_FILTER_H
#define SP_UTIL_COLLECTION_BLOOM_FILTER_H

#include <util/Bitset.h>

namespace sp {
  template <typename T>
  struct Hasher {
  };

  template<typename T,std::size_t indices, typename H = Hasher<T>>
  struct BloomFilter {
    //TODO calc capacity based on indices
    static constexpr capacity = 12;
    std::uint32_t buffer[capacity];

    Bitset bitset;
    BloomFilter() noexcept;
  };

  //TODO impl
  template<typename T>
  bool test(const BloomFilter<T>&, const V&) noexcept;

  template<typename T>
  bool insert(BloomFilter<T>&, const V&) noexcept;

/*
 * =======
 */
}

#endif
