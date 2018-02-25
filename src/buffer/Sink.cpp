#include "Sink.h"

namespace sp {

// Sink::Sink(CircularByteBuffer &b, void *a, SinkFlush s) noexcept
//     : buffer(b)
//     , sink(s)
//     , arg(a) {
// }

std::size_t
push_back(Sink &sink, const unsigned char *w, std::size_t len) noexcept {
  std::size_t written = 0;

  // TODO if buffer is empty and $len > buffer.cap then flush directly fron $w.
Lit:
  written += push_back(sink.buffer, w + written, len - written);
  if (written < len) {
    if (flush(sink)) {
      goto Lit;
    }
  }
  return written;
}

std::size_t
push_back(Sink &sink, BytesView &in) noexcept {
  const unsigned char *w = in.raw + in.pos;
  std::size_t written = write(sink, w, in.length);
  in.pos += written;
  return written;
}

bool
flush(Sink &sink) noexcept {
  if (sink.sink) {
    return sink.sink(sink.buffer, sink.arg);
  }
  return false;
}

} // namespace sp
