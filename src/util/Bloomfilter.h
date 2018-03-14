#ifndef SP_UTIL_COLLECTION_BLOOM_FILTER_H
#define SP_UTIL_COLLECTION_BLOOM_FILTER_H

#include <collection/Array.h>
#include <hash/util.h>
#include <util/Bitset.h>

namespace sp {

// template <typename T, std::size_t N>
// using HasherArray = StaticArray<Hasher<T>, N>;

template <typename T, std::size_t size>
struct BloomFilter {
  StaticBitset<size> bitset;

  Array<Hasher<T>> &hashers;

  explicit BloomFilter(Array<Hasher<T>> &) noexcept;
};

template <typename T, std::size_t s>
bool
test(const BloomFilter<T, s> &, const T &) noexcept;

template <typename T, std::size_t s>
bool
insert(BloomFilter<T, s> &, const T &) noexcept;

/*
 * ==========================================================================
 */
template <typename T, std::size_t size>
BloomFilter<T, size>::BloomFilter(Array<Hasher<T>> &hs) noexcept
    : bitset{}
    , hashers{hs} {
}

template <typename T, std::size_t s>
bool
test(const BloomFilter<T, s> &b, const T &v) noexcept {
  return for_all(b.hashers, [&b, &v](Hasher<T> h) {
    auto hash = h(v);

    std::size_t idx(hash % bits(b.bitset));
    if (!test(b.bitset, idx)) {
      return false;
    }

    return true;
  });
}

template <typename T, std::size_t s>
bool
insert(BloomFilter<T, s> &b, const T &v) noexcept {
  for_each(b.hashers, [&b, &v](Hasher<T> h) {
    auto hash = h(v);
    std::size_t idx(hash % bits(b.bitset));

    set(b.bitset, idx, true);
  });

  return true;
}

} // namespace sp

#endif
