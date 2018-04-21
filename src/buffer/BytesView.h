#ifndef SP_UTIL_BUFFER_BYTE_BUFFER_H
#define SP_UTIL_BUFFER_BYTE_BUFFER_H

#include <cstddef>

namespace sp {

template <typename T>
struct IBytesView {
  T *raw;
  const std::size_t capacity;
  std::size_t length;
  std::size_t pos;

  IBytesView(T *, std::size_t) noexcept;

  // IBytesView(const IBytesView &) = delete;
  IBytesView(const IBytesView<T> &) noexcept;
  IBytesView(const IBytesView<T> &&) = delete;

  IBytesView(IBytesView<T> &, std::size_t, std::size_t) noexcept;

  template <std::size_t SIZE>
  explicit IBytesView(unsigned char (&buffer)[SIZE]) noexcept
      : IBytesView(buffer, SIZE) {
  }

  IBytesView<T> &
  operator=(const IBytesView<T> &) = delete;
  IBytesView<T> &
  operator=(const IBytesView<T> &&) = delete;

  T &operator[](std::size_t) noexcept;
  const T &operator[](std::size_t) const noexcept;
};

using BytesView = IBytesView<unsigned char>;
using ConstBytesView = IBytesView<const unsigned char>;

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
