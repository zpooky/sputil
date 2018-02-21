#include "BytesView.h"
#include "util/numeric.h"
#include <cassert>

namespace sp {
/*BytesView*/
BytesView::BytesView(unsigned char *s, std::size_t l) noexcept
    : raw(s)
    , capacity(l)
    , length(0)
    , pos(0) {
}

BytesView::BytesView(const BytesView &in) noexcept
    : raw(in.raw)
    , capacity(in.capacity)
    , length(in.length)
    , pos(in.pos) {
}

BytesView::BytesView(BytesView &in, std::size_t strt,
                       std::size_t end) noexcept
    : raw(in.raw + strt)
    , capacity(end - strt)
    , length(end - strt)
    , pos(0) {
}

unsigned char &BytesView::operator[](std::size_t idx) noexcept {
  assert(idx < capacity);
  return raw[idx];
}

const unsigned char &BytesView::operator[](std::size_t idx) const noexcept {
  assert(idx < capacity);
  return raw[idx];
}

// BytesView
// copy(BytesView &b) noexcept {
//   return BytesView {
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
  return b.length - b.pos;
}

std::size_t
remaining_write(const BytesView &b) noexcept {
  return b.capacity - b.pos;
}

} // namespace sp
