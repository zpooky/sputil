#include "Thing.h"
#include <util/assert.h>

namespace sp {
//-------------------+---------------
Thing::Thing(CircularByteBuffer &b, void *a, FillType s) noexcept
    : read_head(0)
    , marked(0)
    , buffer(b)
    , fill(s)
    , arg(a) {
}

//-------------------+---------------
ThingMark::ThingMark(std::size_t b, Thing *m) noexcept
    : before(b)
    , marked(m)
    , rollback(false) {
}

ThingMark::ThingMark(ThingMark &&o) noexcept
    : before(o.before)
    , marked(o.marked)
    , rollback(o.rollback) {

  o.marked = nullptr;
}

//-------------------+---------------
static void
do_commit(Thing &self) noexcept {
  assertx(self.marked > 0);
  self.marked--;

  if (self.marked == 0) {
    // TODO make better
    while (self.buffer.read != self.read_head) {
      consume_bytes(self.buffer, 1);
    }
    self.read_head = 0;
  }
}

static void
do_rollback(Thing &self, const ThingMark &mark) noexcept {
  assertx(self.marked > 0);
  self.marked--;

  // should hold specificly when there are nested levels of marks
  assertx(self.read_head >= mark.before);
  self.read_head = mark.before;

  if (self.marked == 0) {
    self.read_head = 0;
    // TDOO assertx(self.read_head == 0);?
  }
}

ThingMark::~ThingMark() noexcept {
  if (marked) {
    // printf("~ThingMark()\n");
    if (rollback) {
      // printf("rollback\n");
      do_rollback(*marked, *this);
    } else {
      // printf("commit\n");
      do_commit(*marked);
    }
    marked = nullptr;
  } else {
    assertx(false);
  }
}

//-------------------+---------------
static bool
is_marked(const Thing &self) noexcept {
  return self.marked > 0;
}

//-------------------+---------------
ThingMark
mark(Thing &self) noexcept {
  std::size_t r = self.marked == 0 ? self.buffer.read : self.read_head;
  if (self.marked == 0) {
    self.read_head = self.buffer.read;
  }

  self.marked++;
  return ThingMark{r, &self};
}

std::size_t
marks(const Thing &self) noexcept {
  return self.marked;
}

//-------------------+---------------
static bool
fill(Thing &self) noexcept {
  if (self.fill) {
    return self.fill(self.buffer, self.arg);
  }
  return true;
}

static void
consume_bytes(Thing &self, std::size_t bytes) {
  if (is_marked(self)) {
    assertx(self.read_head + bytes <= self.buffer.write);
    self.read_head += bytes;
    return;
  }

  return consume_bytes(self.buffer, bytes);
}

template <std::size_t N>
static std::size_t
copy(unsigned char *dest, std::size_t l, StaticArray<BufferSegment, N> &src) {
  std::size_t result = 0;
  for_all(src, [&](auto &segment) {
    if (l > 0) {
      std::size_t a_len = std::min(std::get<1>(segment), l);
      const unsigned char *const arr = std::get<0>(segment);

      std::memcpy(dest + result, arr, a_len);
      result += a_len;
      l -= a_len;
    }
    return l > 0;
  });

  return result;
}

static std::size_t
peek_front_marked(Thing &self, unsigned char *dest, std::size_t len) noexcept {
  if (remaining_read(self.buffer) < len) {
    fill(self);
  }

  if (is_marked(self)) {
    const auto w = self.buffer.write;
    const auto r = self.read_head;
    StaticArray<BufferSegment, 4> result;
    bool ok = impl::read_buffer(self.buffer, result, w, r);
    assertx(ok);
    if (ok) {
      return copy(dest, len, result);
    }

    return 0;
  }

  return peek_front(self.buffer, dest, len);
}

//-------------------+---------------
std::size_t
pop_front(Thing &self, unsigned char *dest, std::size_t l) noexcept {
  auto result = peek_front(self, dest, l);
  consume_bytes(self, result);

  return result;
}

std::size_t
pop_front(Thing &self, BytesView &dest) noexcept {
  const auto l = remaining_write(dest);
  return pop_front(self, dest.raw + dest.pos, l);
}

std::size_t
pop_front(Thing &self, unsigned char &c) noexcept {
  return pop_front(self, &c, 1);
}

//-------------------+---------------
std::size_t
peek_front(Thing &self, unsigned char *dest, std::size_t len) noexcept {
  return peek_front_marked(self, dest, len);
}

std::size_t
peek_front(Thing &self, BytesView &dest) noexcept {
  const auto len = remaining_read(dest);
  const auto read = peek_front(self, dest.raw + dest.pos, len);
  dest.pos += read;
  return read;
}

std::size_t
peek_front(Thing &self, unsigned char &c) noexcept {
  return peek_front(self, &c, 1);
}

//-------------------+---------------
bool
read(Thing &self, void *dest, std::size_t len) noexcept {
  if (len > capacity(self.buffer)) {
    assertx(false);
    return false;
  }

  if (length(self.buffer) < len) {
    if (!fill(self)) {
      return false;
    }
    if (length(self.buffer) < len) {
      return false;
    }
  }

  auto read = (unsigned char *)dest;
  const auto res = peek_front(self, read, len);
  if (res == len) {
    consume_bytes(self, res);
    return true;
  }

  return false;
}

bool
read(Thing &self, unsigned char &dest) noexcept {
  return read(self, &dest, 1);
}

bool
read(Thing &self, char &dest) noexcept {
  return read(self, &dest, 1);
}

//-------------------+---------------
} // namespace sp
