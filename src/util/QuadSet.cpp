#include "QuadSet.h"
#include <type_traits>
#include <util/assert.h>

namespace sp {
static std::size_t
word_index(std::size_t abs_idx) noexcept {
  constexpr std::size_t bits = (sizeof(Quadset::Type) * 8) / 2;
  return abs_idx / bits;
}

static std::size_t
bit_index(std::size_t abs_idx) noexcept {
  constexpr std::size_t bits = (sizeof(Quadset::Type) * 8) / 2;
  return abs_idx % bits;
}

static constexpr Quadset::Type
mask_in_lower() noexcept {
  /* [00000000_11] */
  return Quadset::Type(3);
}

static std::size_t
index_shift(std::size_t idx) noexcept {
  idx = idx * 2;
  return ((sizeof(Quadset::Type) * 8) - idx) - 2;
}

static std::uint8_t
test(typename Quadset::Type word, std::size_t idx) noexcept {
  constexpr auto mask = mask_in_lower();
  std::size_t shift = index_shift(idx);
  return std::uint8_t(mask & (word >> shift));
}

//=====================================
std::uint8_t
test(const Quadset &self, std::size_t idx) noexcept {
  std::size_t wIdx = word_index(idx);
  assertxs(wIdx < self.capacity, wIdx, self.capacity, idx);
  assertxs(self.buffer, self.capacity);

  std::size_t bIdx = bit_index(idx);
  auto word = self.buffer[wIdx];

  return test(word, bIdx);
}

//=====================================

static Quadset::Type
mask_out_index(std::size_t idx) noexcept {
  Quadset::Type result;
  result = mask_in_lower() << index_shift(idx);
  return ~result;
}

static void
set(Quadset::Type &word, std::size_t bit_idx, std::uint8_t v) noexcept {
  const auto mask = mask_out_index(bit_idx);
  word = word & mask;

  Quadset::Type value(v);
  value = value << index_shift(bit_idx);
  word = word | value;
}

std::uint8_t
set(Quadset &self, std::size_t idx, std::uint8_t v) noexcept {
  std::size_t wIdx = word_index(idx);
  assertxs(wIdx < self.capacity, wIdx, self.capacity, idx, v);
  assertxs(self.buffer, self.capacity);
  assertxs(v >= 0 && v <= 3, std::uint32_t(v));

  auto &word = self.buffer[wIdx];
  const auto old_word = word;

  const std::size_t bIdx = bit_index(idx);
  set(word, bIdx, v);

  return test(old_word, bIdx);
}

//=====================================
std::size_t
Quadset_number_of_buffer(std::size_t x) noexcept {
  if (x == 0) {
    return 0;
  }

  constexpr std::size_t buf_size = (sizeof(Quadset::Type) * 8) / 2;
  if (x > buf_size) {
    std::size_t sz = x / buf_size;

    if (x % buf_size > 0) {
      ++sz;
    }

    return sz;
  }

  return 1;
}

//=====================================
void
swap(Quadset &f, Quadset &s) noexcept {
  using std::swap;
  swap(f.buffer, s.buffer);
  swap(f.capacity, s.capacity);
}

//=====================================
Quadset::Quadset(typename Quadset::Type *b, std::size_t c) noexcept
    : buffer{b}
    , capacity{c} {
}

//=====================================
}
