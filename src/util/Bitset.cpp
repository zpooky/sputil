#include "Bitset.h"
#include <type_traits>

namespace sp {
static std::size_t 
word_index(std::size_t absolute_idx) noexcept {
  return 0;//TODO
}

static std::size_t 
bit_index(std::size_t absolute_idx) noexcept {
  return sizeof(Bitset::buffer) % absolute_idx;
}

static std::remove_reference<std::remove_pointer<decltype(Bitset::buffer)>::type>::type
mask_out(std::size_t absolute_idx) noexcept {
  return 0;//TODO
}

Bitset::Bitset(std::uint64_t *b,std::size_t c)noexcept 
  : buffer{b},capacity{c} {
}

bool test(const Bitset&b,std::size_t idx) noexcept {
  std::size_t wIdx = word_index(idx);
  if(wIdx < b.capacity){
    std::size_t bIdx = bit_index(idx);
    auto mask = mask_out(bIdx);
    auto word = b.buffer[wIdx];

    return (mask ^ word) != word;
  }
  return false;
}

bool set(Bitset&, std::size_t, bool) noexcept {
  return true;
}

bool toggle(Bitset&, std::size_t) noexcept {
  return true;
}

}
