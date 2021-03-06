#ifndef SP_UTIL_UTIL_BLOOM_FILTER_H
#define SP_UTIL_UTIL_BLOOM_FILTER_H

#include <collection/Array.h>
#include <hash/util.h>
#include <util/Bitset.h>

// XXX merge two Bloomfilters
namespace sp {
//=====================================
template <typename T, std::size_t size>
struct BloomFilter {
  StaticBitset<size> bitset;

  Array<hasher<T>> &hashers;

  explicit BloomFilter(Array<hasher<T>> &) noexcept;
};

//=====================================
template <typename T, std::size_t s>
bool
test(const BloomFilter<T, s> &, const T &) noexcept;

//=====================================
template <typename T, std::size_t s>
bool
insert(BloomFilter<T, s> &, const T &) noexcept;

//=====================================
template <typename T, std::size_t s>
void
clear(BloomFilter<T, s> &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T, std::size_t size>
BloomFilter<T, size>::BloomFilter(Array<hasher<T>> &hs) noexcept
    : bitset{}
    , hashers{hs} {
}

//=====================================
template <typename T, std::size_t s>
bool
test(const BloomFilter<T, s> &self, const T &v) noexcept {
  return for_all(self.hashers, [&self, &v](hasher<T> h) {
    auto hash = h(v);

    std::size_t idx(hash % bits(self.bitset));
    if (!test(self.bitset, idx)) {
      return false;
    }

    return true;
  });
}

//=====================================
template <typename T, std::size_t s>
bool
insert(BloomFilter<T, s> &self, const T &v) noexcept {
  for_each(self.hashers, [&self, &v](hasher<T> h) {
    auto hash = h(v);
    std::size_t idx(hash % bits(self.bitset));

    set(self.bitset, idx, true);
  });

  // TODO should actually test as well?
  return true;
}

//=====================================
template <typename T, std::size_t s>
void
clear(BloomFilter<T, s> &self) noexcept {
  auto &set = self.bitset;
  std::memset(set.raw, 0, sizeof(set.raw));
}

//=====================================

} // namespace sp

#endif
