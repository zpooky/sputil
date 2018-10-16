#include "RabinKarpHash.h"
#include <util/array.h>
#include <util/assert.h>

namespace sp {
//=====================================
RabinKarpHash::RabinKarpHash(std::size_t c) noexcept
    : length{0}
    , capacity{c}
    , hash{0}
    , buffer{nullptr} {
  buffer = new char[capacity];
}

RabinKarpHash::~RabinKarpHash() noexcept {
  if (buffer) {
    delete[] buffer;
    buffer = nullptr;
  }
}

//=====================================
const char *
update(RabinKarpHash &self, const char *begin, const char *end) noexcept {
  assertxs(self.buffer, self.length, self.capacity);
  assertx(begin < end);

  /* https://github.com/lemire/rollinghashcpp/blob/master/rabinkarphash.h
   * constexpr std::uint32_t B = 37;
   * constexpr std::uint32_t HASHMASK =  
   * 
   * std::uint32_t x = 1;
   * while (begin != end) {
   *   x = (x * B) & HASHMASK;
   * 
   *   if (self.length == self.capacity) {
   *     break;
   *   }
   *   ++self.length;
   * }
   * 
   * x = (x * hasher.hashvalues[c[k]]) & HASHMASK;
   * self.hash = (self.hash + x) & HASHMASK;
   */

  return nullptr;
}

std::size_t
update(RabinKarpHash &self, const char *buffer, std::size_t len) noexcept {
  assertxs(self.buffer, self.length, self.capacity);
  const char *const end = buffer + len;
  const char *const it = update(self, buffer, end);
  return sp::distance(buffer, it);
}

//=====================================
std::uint32_t
hash(const RabinKarpHash &self) noexcept {
  assertxs(self.length == self.capacity, self.length, self.capacity);
  return self.hash;
}

//=====================================
} // namespace sp
