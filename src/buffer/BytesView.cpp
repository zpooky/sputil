#include "BytesView.h"
#include "util/numeric.h"
#include <util/assert.h>

namespace sp {
//=====================================
/*BytesView*/
template <typename T>
IBytesView<T>::IBytesView(T *s, std::size_t cap) noexcept
    : raw{s}
    , capacity{cap}
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
T &
IBytesView<T>::operator[](std::size_t idx) noexcept {
  assertx(idx < capacity);
  return raw[idx];
}

template <typename T>
const T &
IBytesView<T>::operator[](std::size_t idx) const noexcept {
  assertx(idx < capacity);
  return raw[idx];
}

//=====================================
BytesViewMark::BytesViewMark(std::size_t b, BytesView *m) noexcept
    : before(b)
    , marked(m)
    , rollback(false) {
}

BytesViewMark::BytesViewMark(BytesViewMark &&o) noexcept
    : before(o.before)
    , marked(o.marked)
    , rollback(o.rollback) {
  o.marked = nullptr;
}

BytesViewMark::~BytesViewMark() noexcept {
  if (rollback) {
    assertx(marked);
    marked->pos = before;
  }
}

//=====================================
// IBytesView
// copy(IBytesView &b) noexcept {
//   return IBytesView {
//     b.raw
//   }
// }

//=====================================
void
flip(BytesView &b) noexcept {
  using sp::swap;
  swap(b.length, b.pos);
}

//=====================================
void
reset(BytesView &b) noexcept {
  b.length = 0;
  b.pos = 0;
}

//=====================================
unsigned char *
offset(BytesView &b) noexcept {
  return b.raw + b.pos;
}

//=====================================
std::size_t
remaining_read(const BytesView &b) noexcept {
  assertx(b.length >= b.pos);
  assertx(b.length <= b.capacity);

  if (b.length < b.pos) {
    return 0;
  }

  return b.length - b.pos;
}

//=====================================
std::size_t
remaining_write(const BytesView &b) noexcept {
  assertx(b.capacity >= b.pos);
  return b.capacity - b.pos;
}

//=====================================
std::size_t
pop_front(BytesView &self, unsigned char &out) noexcept {
  if (remaining_read(self) > 0) {
    out = self.raw[self.pos++];
    return 1;
  }
  return 0;
}

std::size_t
pop_front(BytesView &self, char &out) noexcept {
  if (remaining_read(self) > 0) {
    out = (char)self.raw[self.pos++];
    return 1;
  }
  return 0;
}

std::size_t
pop_front(BytesView &self, void *tmp_out, std::size_t l) noexcept {
  unsigned char *out = (unsigned char *)tmp_out;
  std::size_t read = std::min(remaining_read(self), l);
  std::memcpy(out, self.raw + self.pos, read);
  self.pos += read;
  return read;
}

//=====================================
std::size_t
peek_front(const BytesView &self, unsigned char &out) noexcept {
  if (remaining_read(self) > 0) {
    out = self.raw[self.pos];
    return 1;
  }
  return 0;
}

std::size_t
peek_front(const BytesView &self, char &out) noexcept {
  if (remaining_read(self) > 0) {
    out = (char)self.raw[self.pos];
    return 1;
  }
  return 0;
}

std::size_t
peek_front(const BytesView &self, unsigned char *out, std::size_t l) noexcept {
  std::size_t read = std::min(remaining_read(self), l);
  std::memcpy(out, self.raw + self.pos, read);
  return read;
}

std::size_t
peek_front(const BytesView &self, char *out, std::size_t l) noexcept {
  std::size_t read = std::min(remaining_read(self), l);
  std::memcpy(out, self.raw + self.pos, read);
  return read;
}

//=====================================
bool
write(BytesView &self, const void *tmp_in, std::size_t l) noexcept {
  const unsigned char *in = (const unsigned char *)tmp_in;
  if (remaining_write(self) < l) {
    return false;
  }
  std::memcpy(self.raw + self.pos, in, l);
  self.pos += l;

  return true;
}

bool
write(BytesView &self, char in) noexcept {
  return write(self, &in, 1);
}

//=====================================
BytesViewMark
mark(BytesView &self) noexcept {
  return BytesViewMark{self.pos, &self};
}

//=====================================
template struct IBytesView<unsigned char>;
template struct IBytesView<const unsigned char>;
template struct IBytesView<char>;
template struct IBytesView<const char>;

} // namespace sp
