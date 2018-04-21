#include "BytesView.h"
#include "util/numeric.h"
#include <util/assert.h>

namespace sp {
/*BytesView*/
template <typename T>
IBytesView<T>::IBytesView(T *s, std::size_t l) noexcept
    : raw{s}
    , capacity{l}
    , length{0}
    , pos{0} {
}

template <typename T>
IBytesView<T>::IBytesView(const IBytesView<T> &in) noexcept
    : raw(in.raw)
    , capacity(in.capacity)
    , length(in.length)
    , pos(in.pos) {
}

template <typename T>
IBytesView<T>::IBytesView(IBytesView<T> &in, std::size_t strt,
                          std::size_t end) noexcept
    : raw(in.raw + strt)
    , capacity(end - strt)
    , length(end - strt)
    , pos(0) {
  assertx(end >= strt);
}

template <typename T>
T &IBytesView<T>::operator[](std::size_t idx) noexcept {
  assertx(idx < capacity);
  return raw[idx];
}

template <typename T>
const T &IBytesView<T>::operator[](std::size_t idx) const noexcept {
  assertx(idx < capacity);
  return raw[idx];
}

// IBytesView
// copy(IBytesView &b) noexcept {
//   return IBytesView {
//     b.raw
//   }
// }

void
flip(BytesView &b) noexcept {
  using sp::swap;
  swap(b.length, b.pos);
}

void
reset(BytesView &b) noexcept {
  b.length = 0;
  b.pos = 0;
}

unsigned char *
offset(BytesView &b) noexcept {
  return b.raw + b.pos;
}

std::size_t
remaining_read(const BytesView &b) noexcept {
  assertx(b.length >= b.pos);
  if (b.length < b.pos) {
    return 0;
  }

  return b.length - b.pos;
}

std::size_t
remaining_write(const BytesView &b) noexcept {
  assertx(b.capacity >= b.pos);
  return b.capacity - b.pos;
}

template struct IBytesView<unsigned char>;
template struct IBytesView<const unsigned char>;
template struct IBytesView<char>;
template struct IBytesView<const char>;

} // namespace sp
