#ifndef SP_UTIL_BUFFER_SINK_H
#define SP_UTIL_BUFFER_SINK_H

#include <buffer/BytesView.h>
#include <buffer/CircularByteBuffer.h>
// TODO impl

namespace sp {
struct Sink {
  using FlushType = bool (*)(CircularByteBuffer &, void *) noexcept;
  CircularByteBuffer &buffer;
  FlushType sink;
  void *arg;

  explicit Sink(CircularByteBuffer &b, void *a, FlushType s) noexcept
      : buffer(b)
      , sink(s)
      , arg(a) {
  }
};

//-------------------+---------------
bool
write(Sink &, const unsigned char *, std::size_t) noexcept;

template <std::size_t SIZE>
bool
write(Sink &s, const unsigned char (&buffer)[SIZE]) noexcept {
  return write(s, buffer, SIZE);
}

bool
write(Sink &, BytesView &) noexcept;
//-------------------+---------------
/*
 * returns byte written
 */
std::size_t
push_back(Sink &, const unsigned char *, std::size_t) noexcept;

template <std::size_t SIZE>
std::size_t
push_back(Sink &s, const unsigned char (&buffer)[SIZE]) noexcept {
  return push_back(s, buffer, SIZE);
}

std::size_t
push_back(Sink &, BytesView &) noexcept;

//-------------------+---------------
bool
flush(Sink &) noexcept;

} // namespace sp
#endif
