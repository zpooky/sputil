#include "ByteBuffer.h"
#include "util/numeric.h"
#include <cassert>

namespace sp {
/*ByteBuffer*/
ByteBuffer::ByteBuffer(unsigned char *s, std::size_t l) noexcept
    : raw(s)
    , capacity(l)
    , length(0)
    , pos(0) {
}

ByteBuffer::ByteBuffer(const ByteBuffer &in) noexcept
    : raw(in.raw)
    , capacity(in.capacity)
    , length(in.length)
    , pos(in.pos) {
}

ByteBuffer::ByteBuffer(ByteBuffer &in, std::size_t strt,
                       std::size_t end) noexcept
    : raw(in.raw + strt)
    , capacity(end - strt)
    , length(end - strt)
    , pos(0) {
}

unsigned char &ByteBuffer::operator[](std::size_t idx) noexcept {
  assert(idx < capacity);
  return raw[idx];
}

const unsigned char &ByteBuffer::operator[](std::size_t idx) const noexcept {
  assert(idx < capacity);
  return raw[idx];
}

// ByteBuffer
// copy(ByteBuffer &b) noexcept {
//   return ByteBuffer {
//     b.raw
//   }
// }

void
flip(ByteBuffer &b) noexcept {
  using sp::swap;
  swap(b.length, b.pos);
}

void
reset(ByteBuffer &b) noexcept {
  b.length = 0;
  b.pos = 0;
}

unsigned char *
offset(ByteBuffer &b) noexcept {
  return b.raw + b.pos;
}

std::size_t
remaining_read(const ByteBuffer &b) noexcept {
  return b.length - b.pos;
}

std::size_t
remaining_write(const ByteBuffer &b) noexcept {
  return b.capacity - b.pos;
}

} // namespace sp
