#ifndef SP_UTIL_BUFFER_CIRCULAR_BYTE_BUFFER_H
#define SP_UTIL_BUFFER_CIRCULAR_BYTE_BUFFER_H

#include "buffer/BytesView.h"
#include <cstddef>

namespace sp {
struct CircularByteBuffer {
  unsigned char *buffer;
  std::size_t read;
  std::size_t write;
  const std::size_t capacity;

  CircularByteBuffer(unsigned char *, std::size_t) noexcept;
};

template <std::size_t SIZE>
struct StaticCircularByteBuffer : public CircularByteBuffer {
  unsigned char raw[SIZE];

  StaticCircularByteBuffer() noexcept;
};

std::size_t
length(const CircularByteBuffer &) noexcept;

bool
is_empty(const CircularByteBuffer &) noexcept;

bool
is_full(const CircularByteBuffer &) noexcept;

std::size_t
remaining_write(const CircularByteBuffer &) noexcept;

std::size_t
remaining_read(const CircularByteBuffer &) noexcept;

std::size_t
push_back(CircularByteBuffer &, BytesView &) noexcept;

std::size_t
push_back(CircularByteBuffer &, const unsigned char *, std::size_t) noexcept;

template <std::size_t SIZE>
std::size_t
push_back(CircularByteBuffer &b, const unsigned char (&buffer)[SIZE]) noexcept {
  return push_back(b, buffer, SIZE);
}

std::size_t
pop_front(CircularByteBuffer &, BytesView &) noexcept;

std::size_t
pop_front(CircularByteBuffer &, unsigned char *, std::size_t) noexcept;

template <std::size_t SIZE>
std::size_t
pop_front(CircularByteBuffer &self, unsigned char (&buffer)[SIZE]) noexcept {
  return pop_front(self, buffer, SIZE);
}
/*
 * ==========================================================================
 */

template <std::size_t SIZE>
StaticCircularByteBuffer<SIZE>::StaticCircularByteBuffer() noexcept
    : CircularByteBuffer(raw, SIZE)
    , raw() {
  static_assert((SIZE & (SIZE - 1)) == 0, "required to be power of 2");
}

} // namespace sp

#endif
