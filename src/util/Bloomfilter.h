#ifndef SP_UTIL_UTIL_BLOOM_FILTER_H
#define SP_UTIL_UTIL_BLOOM_FILTER_H

#include <algorithm>
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
  size_t unique_inserts;

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
template <typename T, std::size_t s>
size_t
theoretical_max_capacity(const BloomFilter<T, s> &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T, std::size_t size>
BloomFilter<T, size>::BloomFilter(Array<hasher<T>> &hs) noexcept
    : bitset{}
    , hashers{hs}
    , unique_inserts(0) {
}

//=====================================
template <typename T, std::size_t s>
bool
test(const BloomFilter<T, s> &self, const T &v) noexcept {
  assertx(!is_empty(self.hashers));
  return for_all(self.hashers, [&self, &v](hasher<T> h) {
    auto hash = h(v);

    std::size_t idx(hash % capacity(self.bitset));
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
  assertx(!is_empty(self.hashers));
  bool already_present = true;
  for_each(self.hashers, [&self, &v, &already_present](hasher<T> h) {
    auto hash = h(v);
    std::size_t idx(hash % capacity(self.bitset));

    already_present &= test(self.bitset, idx);
    set(self.bitset, idx, true);
  });

  if (!already_present) {
    self.unique_inserts++;
  }

  return already_present;
}

//=====================================
template <typename T, std::size_t s>
void
clear(BloomFilter<T, s> &self) noexcept {
  auto &set = self.bitset;
  std::memset(set.raw, 0, sizeof(set.raw));
  self.unique_inserts = 0;
}

//=====================================
template <typename T, std::size_t s>
size_t
theoretical_max_capacity(const BloomFilter<T, s> &self) noexcept {
  return capacity(self.bitset) / std::max(length(self.hashers), std::size_t(1));
}

//=====================================

} // namespace sp

#endif
