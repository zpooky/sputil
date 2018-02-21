#ifndef SP_UTIL_BUFFER_BYTE_BUFFER_H
#define SP_UTIL_BUFFER_BYTE_BUFFER_H

#include <cstddef>

namespace sp {

struct BytesView {
  unsigned char *raw;
  const std::size_t capacity;
  std::size_t length;
  std::size_t pos;

  BytesView(unsigned char *, std::size_t) noexcept;

  // BytesView(const BytesView &) = delete;
  BytesView(const BytesView &) noexcept;
  BytesView(const BytesView &&) = delete;

  BytesView(BytesView &, std::size_t, std::size_t) noexcept;

  template <std::size_t SIZE>
  explicit BytesView(unsigned char (&buffer)[SIZE]) noexcept
      : BytesView(buffer, SIZE) {
  }

  BytesView &
  operator=(const BytesView &) = delete;
  BytesView &
  operator=(const BytesView &&) = delete;

  unsigned char &operator[](std::size_t) noexcept;

  const unsigned char &operator[](std::size_t) const noexcept;
};

template <std::size_t SIZE>
struct StaticBytesView : public BytesView {
  static constexpr std::size_t storage_capacity = SIZE;
  unsigned char r[SIZE];
  StaticBytesView();
};

// BytesView
// copy(BytesView &) noexcept;

void
flip(BytesView &) noexcept;

void
reset(BytesView &) noexcept;

unsigned char *
offset(BytesView &) noexcept;

std::size_t
remaining_read(const BytesView &) noexcept;

std::size_t
remaining_write(const BytesView &) noexcept;
/*
 * ==========================================================================
 */

template <std::size_t c>
StaticBytesView<c>::StaticBytesView()
    : BytesView(r, c)
    , r{} {
}

} // namespace sp

#endif
