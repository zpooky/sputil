#ifndef SP_UTIL_BUFFER_THING_H
#define SP_UTIL_BUFFER_THING_H

#include <buffer/BytesView.h>
#include <buffer/CircularByteBuffer.h>
#include <cstddef>

namespace sp {
//=====================================
struct Thing {
  using FillType = bool (*)(CircularByteBuffer &, void *);

  // The index of where we are reading from
  // - on rollback this is discarded
  // - on commit
  //  - [0,4,7]?
  std::size_t read_head;
  // 0: not marked
  // n: marked
  std::size_t marked;

  CircularByteBuffer &buffer;

  FillType fill;
  void *arg;

  Thing(CircularByteBuffer &, void *, FillType) noexcept;

  Thing(const Thing &) = delete;
  Thing(const Thing &&) = delete;
};

//=====================================
struct ThingMark {
  const std::size_t before;
  Thing *marked;
  bool rollback;

  // RAII
  ThingMark(std::size_t, Thing *) noexcept;

  ThingMark(const ThingMark &) = delete;
  ThingMark(ThingMark &&) noexcept;

  ~ThingMark() noexcept;
};

//=====================================
// TODO mark should be on CBBuffer level instead....
/* Mark current position */
ThingMark
mark(Thing &) noexcept;

std::size_t
marks(const Thing &) noexcept;

bool
is_marked(const Thing &) noexcept;

//=====================================
// ConstBytesView
// peek() noexcept;

std::size_t
pop_front(Thing &, unsigned char *, std::size_t) noexcept;

template <std::size_t SIZE>
std::size_t
pop_front(Thing &self, unsigned char (&buffer)[SIZE]) noexcept {
  return pop_front(self, buffer, SIZE);
}

#if 0
std::size_t
pop_front(Thing &, char *, std::size_t) noexcept;

template <std::size_t SIZE>
std::size_t
pop_front(Thing &self, char (&buffer)[SIZE]) noexcept {
  return pop_front(self, buffer, SIZE);
}
#endif

std::size_t
pop_front(Thing &, BytesView &) noexcept;

std::size_t
pop_front(Thing &, unsigned char &) noexcept;

//=====================================
std::size_t
peek_front(Thing &, /*copy DEST*/ unsigned char *, std::size_t) noexcept;

template <std::size_t SIZE>
std::size_t
peek_front(Thing &self, unsigned char (&buffer)[SIZE]) noexcept {
  return peek_front(self, buffer, SIZE);
}

#if 0
std::size_t
peek_front(Thing &, /*copy DEST*/ char *, std::size_t) noexcept;

template <std::size_t SIZE>
std::size_t
peek_front(Thing &self, char (&buffer)[SIZE]) noexcept {
  return peek_front(self, buffer, SIZE);
}
#endif

std::size_t
peek_front(Thing &, /*copy DEST*/ BytesView &) noexcept;

std::size_t
peek_front(Thing &self, unsigned char &c) noexcept;

//=====================================
bool
read(Thing &, void *, std::size_t) noexcept;

bool
read(Thing &, unsigned char &) noexcept;

bool
read(Thing &, char &) noexcept;

template <std::size_t SIZE>
bool
read(Thing &self, unsigned char (&buffer)[SIZE]) noexcept {
  return read(self, buffer, SIZE);
}

template <std::size_t SIZE>
bool
read(Thing &self, char (&buffer)[SIZE]) noexcept {
  return read(self, buffer, SIZE);
}

//=====================================
} // namespace sp

#endif
