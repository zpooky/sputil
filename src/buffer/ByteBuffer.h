#ifndef SP_UTIL_BUFFER_BYTE_BUFFER_H
#define SP_UTIL_BUFFER_BYTE_BUFFER_H

#include <cstddef>

namespace sp {

struct ByteBuffer {
  unsigned char *raw;
  const std::size_t capacity;
  std::size_t length;
  std::size_t pos;

  ByteBuffer(unsigned char *, std::size_t) noexcept;

  // ByteBuffer(const ByteBuffer &) = delete;
  ByteBuffer(const ByteBuffer &) noexcept;
  ByteBuffer(const ByteBuffer &&) = delete;

  ByteBuffer(ByteBuffer &, std::size_t, std::size_t) noexcept;

  template <std::size_t SIZE>
  explicit ByteBuffer(unsigned char (&buffer)[SIZE]) noexcept
      : ByteBuffer(buffer, SIZE) {
  }

  ByteBuffer &
  operator=(const ByteBuffer &) = delete;
  ByteBuffer &
  operator=(const ByteBuffer &&) = delete;

  unsigned char &operator[](std::size_t) noexcept;

  const unsigned char &operator[](std::size_t) const noexcept;
};

template <std::size_t SIZE>
struct StaticByteBuffer : public ByteBuffer {
  static constexpr std::size_t storage_capacity = SIZE;
  unsigned char r[SIZE];
  StaticByteBuffer();
};

// ByteBuffer
// copy(ByteBuffer &) noexcept;

void
flip(ByteBuffer &) noexcept;

void
reset(ByteBuffer &) noexcept;

unsigned char *
offset(ByteBuffer &) noexcept;

std::size_t
remaining_read(const ByteBuffer &) noexcept;

std::size_t
remaining_write(const ByteBuffer &) noexcept;
/*
 * ==========================================================================
 */

template <std::size_t c>
StaticByteBuffer<c>::StaticByteBuffer()
    : ByteBuffer(r, c)
    , r{} {
}

} // namespace sp

#endif
