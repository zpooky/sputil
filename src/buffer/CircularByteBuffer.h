#ifndef SP_UTIL_BUFFER_CIRCULAR_BYTE_BUFFER_H
#define SP_UTIL_BUFFER_CIRCULAR_BYTE_BUFFER_H

#include "buffer/BytesView.h"
#include <cassert>
#include <collection/Array.h>
#include <cstddef>
#include <tuple>

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
remaining_write(const CircularByteBuffer &) noexcept;

std::size_t
remaining_read(const CircularByteBuffer &) noexcept;

std::size_t
length(const CircularByteBuffer &) noexcept;

std::size_t
capacity(const CircularByteBuffer &) noexcept;

bool
is_empty(const CircularByteBuffer &) noexcept;

bool
is_full(const CircularByteBuffer &) noexcept;

void
reset(CircularByteBuffer &) noexcept;

std::size_t
push_back(CircularByteBuffer &, BytesView &) noexcept;

std::size_t
push_back(CircularByteBuffer &, unsigned char) noexcept;

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

std::size_t
pop_front(CircularByteBuffer &, unsigned char &) noexcept;

template <std::size_t SIZE>
std::size_t
pop_front(CircularByteBuffer &self, unsigned char (&buffer)[SIZE]) noexcept {
  return pop_front(self, buffer, SIZE);
}

std::size_t
peek_front(const CircularByteBuffer &, BytesView &) noexcept;

std::size_t
peek_front(const CircularByteBuffer &, unsigned char *, std::size_t) noexcept;

std::size_t
peek_front(const CircularByteBuffer &, unsigned char &) noexcept;

template <std::size_t SIZE>
std::size_t
peek_front(const CircularByteBuffer &self, unsigned char (&buffer)[SIZE]) noexcept {
  return peek_front(self, buffer, SIZE);
}

void
consume_bytes(CircularByteBuffer &, std::size_t) noexcept;

bool
read_buffer(CircularByteBuffer &,
            Array<std::tuple<unsigned char *, std::size_t>> &) noexcept;
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
