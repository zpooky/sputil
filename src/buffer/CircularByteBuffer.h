#ifndef SP_UTIL_BUFFER_CIRCULAR_BYTE_BUFFER_H
#define SP_UTIL_BUFFER_CIRCULAR_BYTE_BUFFER_H

#include "buffer/BytesView.h"
#include <collection/Array.h>
#include <cstddef>
#include <tuple>

namespace sp {
//=====================================
struct CircularByteBuffer {
  uint8_t *buffer;
  std::size_t read;
  std::size_t write;
  const std::size_t capacity;

  using BufferArray = StaticArray<std::tuple<uint8_t *, std::size_t>, 2>;

  CircularByteBuffer(uint8_t *, std::size_t) noexcept;
};

// TODO support write marks and rollback to that point. Stop read when mark
// encountered?

//=====================================
template <std::size_t SIZE>
struct StaticCircularByteBuffer : public CircularByteBuffer {
  uint8_t raw[SIZE];

  StaticCircularByteBuffer() noexcept;
};

//=====================================
std::size_t
remaining_write(const CircularByteBuffer &) noexcept;

std::size_t
remaining_read(const CircularByteBuffer &) noexcept;

//=====================================
std::size_t
length(const CircularByteBuffer &) noexcept;

std::size_t
capacity(const CircularByteBuffer &) noexcept;

//=====================================
bool
is_empty(const CircularByteBuffer &) noexcept;

bool
is_full(const CircularByteBuffer &) noexcept;

//=====================================
void
reset(CircularByteBuffer &) noexcept;

//=====================================
std::size_t
push_back(CircularByteBuffer &, BytesView &) noexcept;

std::size_t
push_back(CircularByteBuffer &, uint8_t) noexcept;

std::size_t
push_back(CircularByteBuffer &, char) noexcept;

std::size_t
push_back(CircularByteBuffer &, const void *, std::size_t) noexcept;

template <std::size_t SIZE>
std::size_t
push_back(CircularByteBuffer &b, const uint8_t (&buffer)[SIZE]) noexcept {
  return push_back(b, buffer, SIZE);
}

//=====================================
bool
write(CircularByteBuffer &, const void *, std::size_t) noexcept;

bool
write(CircularByteBuffer &, uint8_t) noexcept;

bool
write(CircularByteBuffer &, char) noexcept;

template <std::size_t SIZE>
bool
write(CircularByteBuffer &b, const uint8_t (&buffer)[SIZE]) noexcept {
  return write(b, buffer, SIZE);
}

template <std::size_t SIZE>
bool
write(CircularByteBuffer &b, const char (&buffer)[SIZE]) noexcept {
  return write(b, buffer, SIZE);
}

//=====================================
std::size_t
pop_front(CircularByteBuffer &, BytesView &) noexcept;

std::size_t
pop_front(CircularByteBuffer &, uint8_t *, std::size_t) noexcept;

std::size_t
pop_front(CircularByteBuffer &, uint8_t &) noexcept;

template <std::size_t SIZE>
std::size_t
pop_front(CircularByteBuffer &self, uint8_t (&buffer)[SIZE]) noexcept {
  return pop_front(self, buffer, SIZE);
}

//=====================================
std::size_t
peek_front(const CircularByteBuffer &, /*copy DEST*/ BytesView &) noexcept;

std::size_t
peek_front(const CircularByteBuffer &, /*copy DEST*/ uint8_t *,
           std::size_t) noexcept;

std::size_t
peek_front(const CircularByteBuffer &, uint8_t &) noexcept;

template <std::size_t SIZE>
std::size_t
peek_front(const CircularByteBuffer &self, uint8_t (&buffer)[SIZE]) noexcept {
  return peek_front(self, buffer, SIZE);
}

//=====================================
bool
read(CircularByteBuffer &, void *, std::size_t) noexcept;

bool
read(CircularByteBuffer &, uint8_t &) noexcept;

bool
read(CircularByteBuffer &, char &) noexcept;

template <std::size_t SIZE>
bool
read(CircularByteBuffer &b, uint8_t (&buffer)[SIZE]) noexcept {
  return write(b, buffer, SIZE);
}

template <std::size_t SIZE>
bool
read(CircularByteBuffer &b, char (&buffer)[SIZE]) noexcept {
  return write(b, buffer, SIZE);
}

//=====================================
// ConstBytesView
// peek_front(const CircularByteBuffer &self) noexcept;

//=====================================
using BufferSegment = std::tuple<uint8_t *, std::size_t>;
using ConstBufferSegment = std::tuple<const uint8_t *, std::size_t>;
/* Used together with read_buffer() to signify how many bytes read */
void
consume_bytes(CircularByteBuffer &, std::size_t) noexcept;

bool
read_buffer(CircularByteBuffer &, Array<BufferSegment> &) noexcept;

bool
read_buffer(const CircularByteBuffer &, Array<ConstBufferSegment> &) noexcept;

//=====================================
/* Used together with write_buffer() to signify how many bytes written */
void
produce_bytes(CircularByteBuffer &self, std::size_t b) noexcept;

bool
write_buffer(CircularByteBuffer &, Array<BufferSegment> &) noexcept;

bool
write_buffer(const CircularByteBuffer &, Array<ConstBufferSegment> &) noexcept;

//=====================================
template <std::size_t SIZE>
StaticCircularByteBuffer<SIZE>::StaticCircularByteBuffer() noexcept
    : CircularByteBuffer(raw, SIZE)
    , raw() {
  static_assert((SIZE & (SIZE - 1)) == 0, "required to be power of 2");
}

//=====================================
namespace impl {
std::size_t
cbb_remaining_read(std::size_t write, std::size_t read) noexcept;

bool
read_buffer(CircularByteBuffer &self, Array<BufferSegment> &result,
            std::size_t w, std::size_t r) noexcept;
bool
read_buffer(const CircularByteBuffer &s, Array<ConstBufferSegment> &res,
            std::size_t w, std::size_t r) noexcept;
//=====================================
} // namespace impl
} // namespace sp

#endif
