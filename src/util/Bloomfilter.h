#ifndef SP_UTIL_COLLECTION_BLOOM_FILTER_H
#define SP_UTIL_COLLECTION_BLOOM_FILTER_H

#include <util/Bitset.h>

namespace sp {
  //TODO move to hash/??
  template<typename T>
  using Hasher = std::size_t (*)(const T&);

  template<typename T, std::size_t size>
  struct BloomFilter {
    std::uint64_t buffer[size];
    Bitset bitset;

    Hasher<T>* hashers;
    const std::size_t length;

    BloomFilter(Hasher<T>*,std::size_t) noexcept;

    template<std::size_t SIZE>
    BloomFilter(Hasher<T>(&)[SIZE]) noexcept;
  };

  template<typename T,std::size_t s>
  bool test(const BloomFilter<T,s>&, const T&) noexcept;

  template<typename T,std::size_t s>
  bool insert(BloomFilter<T,s>&, const T&) noexcept;

/*
 * ==========================================================================
 */
template<typename T, std::size_t size>
BloomFilter<T,size>::BloomFilter(Hasher<T>*hs,std::size_t len) noexcept 
  : buffer{0}
  , bitset{buffer}
  , hashers{hs}
  , length{len} {
}

template<typename T, std::size_t size>
template<std::size_t SIZE>
BloomFilter<T,size>::BloomFilter(Hasher<T>(&buf)[SIZE]) noexcept 
  : BloomFilter(buf,SIZE){
}

template<typename T,std::size_t s>
bool test(const BloomFilter<T,s>&b, const T&v) noexcept {
  for(std::size_t i=0;i<b.length;++i){
    auto hash = b.hashers[i](v);
    std::size_t idx(hash % bits(b.bitset));
    if(!test(b.bitset,idx)){
      return false;
    }
  }

  return true;
}

template<typename T,std::size_t s>
bool insert(BloomFilter<T,s>&b, const T&v) noexcept {
  for(std::size_t i=0;i<b.length;++i){
    auto hash = b.hashers[i](v);
    std::size_t idx(hash % bits(b.bitset));
    set(b.bitset, idx, true);
  }

  return true;
}

}

#endif
