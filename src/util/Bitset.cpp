#include "Bitset.h"
#include <type_traits>
#include <cassert>

namespace sp {
struct Bitset_buffer {
  using type =std::remove_reference<std::remove_pointer<decltype(Bitset::buffer)>::type>::type;
};

static std::size_t 
word_index(std::size_t absolute_idx) noexcept {
  std::size_t bits = sizeof(Bitset_buffer::type)*8;
  return absolute_idx / bits;
}

static std::size_t 
bit_index(std::size_t absolute_idx) noexcept {
  std::size_t bits = sizeof(Bitset_buffer::type)*8;
  return absolute_idx%bits;
}

static Bitset_buffer::type
mask_out(std::size_t idx) noexcept {
  typename Bitset_buffer::type result =0;
  return  (result | 1) << idx ;
}

static bool 
test(typename Bitset_buffer::type word, std::size_t bit_idx) {
  auto mask = mask_out(bit_idx);
  return (mask & word) == mask;
}

static void
set(typename Bitset_buffer::type &word, std::size_t bit_idx, bool v) {
  const auto mask = mask_out(bit_idx);
  if(v){
    word = word | mask;
  } else {
    word = word & (~mask);
  }
}

Bitset::Bitset(std::uint64_t *b,std::size_t c)noexcept 
  : buffer{b},capacity{c} {
    for(std::size_t i=0;i<c;++i){
      assert(*b == 0);
    }
}

bool test(const Bitset&b,std::size_t idx) noexcept {
  std::size_t wIdx = word_index(idx);
  assert(wIdx < b.capacity);

  std::size_t bIdx = bit_index(idx);
  auto word = b.buffer[wIdx];

  return test(word,bIdx);
}

bool set(Bitset&b, std::size_t idx, bool v) noexcept {
  std::size_t wIdx = word_index(idx);
  assert(wIdx < b.capacity);

  const std::size_t bIdx = bit_index(idx);
  auto &word = b.buffer[wIdx];
  const auto old_word = word;
  set(word,bIdx,v);

  return test(old_word,bIdx);
}

bool toggle(Bitset&b, std::size_t idx) noexcept {
  std::size_t wIdx = word_index(idx);
  assert(wIdx < b.capacity);

  const std::size_t bIdx = bit_index(idx);
  auto &word = b.buffer[wIdx];

  const bool v = !test(word, bIdx);
  set(word,bIdx,v);

  return v;
}

std::size_t bits(const Bitset&b) noexcept {
  return sizeof(Bitset_buffer::type)*8*b.capacity;
}

}
