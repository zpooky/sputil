#include "Sink.h"
#include <util/assert.h>

namespace sp {

// Sink::Sink(CircularByteBuffer &b, void *a, SinkFlush s) noexcept
//     : buffer(b)
//     , sink(s)
//     , arg(a) {
// }

Sink::~Sink() {
  // assertx(flush(*this));
  flush(*this);
}
//-------------------+---------------
static bool
write(Sink &sink, const unsigned char *w, std::size_t l) {
  if (l > capacity(sink.buffer)) {
    if (sink.sink == nullptr) {
      assertx(false);
      return false;
    }

    if (!flush(sink)) {
      return false;
    }

    // TODO something like this
    // CircularByteBuffer dummy(w,l)
    // return sink.sink(dummy);
    return false;
  } else if (l > remaining_write(sink.buffer)) {
    if (!flush(sink)) {
      return false;
    }
  }

  if (!write(sink.buffer, w, l)) {
    return false;
  }
  return true;
}

bool
write(Sink &sink, BytesView &in) noexcept {
  auto length = remaining_read(in);
  if (!write(sink, offset(in), length)) {
    return false;
  }
  in.pos += length;
  return true;
}

bool
write(Sink &sink, const void *w, std::size_t l) noexcept {
  return write(sink, (unsigned char *)w, l);
}

bool
write(Sink &sink, unsigned char c) noexcept {
  return write(sink, &c, 1);
}

bool
write(Sink &sink, char c) noexcept {
  return write(sink, &c, 1);
}

//-------------------+---------------

static std::size_t
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
push_back(Sink &sink, const void *w, std::size_t len) noexcept {
  return push_back(sink, (const unsigned char *)w, len);
}

bool
push_back(Sink &sink, unsigned char c) noexcept {
  return push_back(sink, &c, 1);
}

bool
push_back(Sink &sink, char c) noexcept {
  return push_back(sink, &c, 1);
}

std::size_t
push_back(Sink &sink, BytesView &in) noexcept {
  const unsigned char *w = in.raw + in.pos;
  std::size_t written = push_back(sink, w, in.length);
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
