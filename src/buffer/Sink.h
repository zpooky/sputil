#ifndef SP_UTIL_BUFFER_SINK_H
#define SP_UTIL_BUFFER_SINK_H

#include <buffer/BytesView.h>
#include <buffer/CircularByteBuffer.h>
// TODO impl

namespace sp {
using SinkFlush = bool (*)(CircularByteBuffer &, void *) noexcept;
struct Sink {
  CircularByteBuffer &buffer;
  SinkFlush sink;
  void *arg;

  explicit Sink(CircularByteBuffer &b, void *a, SinkFlush s) noexcept
      : buffer(b)
      , sink(s)
      , arg(a) {
  }
};

/*
 * ==========================================================================
 */

// TODO write -> bool

/* returns byte written*/
std::size_t
push_back(Sink &, const unsigned char *, std::size_t) noexcept;

template <std::size_t SIZE>
std::size_t
push_back(Sink &s, const unsigned char (&buffer)[SIZE]) noexcept {
  return push_back(s, buffer, SIZE);
}

std::size_t
push_back(Sink &, BytesView &) noexcept;

bool
flush(Sink &) noexcept;

} // namespace sp
#endif
