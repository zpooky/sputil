#ifndef SP_UTIL_BUFFER_SINK_H
#define SP_UTIL_BUFFER_SINK_H

#include <buffer/CircularByteBuffer.h>
// TODO impl

namespace sp {
struct Sink {
  CircularByteBuffer &buffer;
  bool (*sink)(CircularByteBuffer &) noexcept;
  explicit Sink(CircularByteBuffer &) noexcept;
};

/*
 * ==========================================================================
 */

/* returns byte written*/
std::size_t
write(Sink &, const unsigned char *, std::size_t) noexcept;

template <std::size_t SIZE>
std::size_t
write(Sink &s, const unsigned char (&buffer)[SIZE]) noexcept {
  return write(s, buffer, SIZE);
}

bool
flush(Sink &) noexcept;

} // namespace sp
#endif
