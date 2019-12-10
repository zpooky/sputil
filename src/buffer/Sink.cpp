#include "Sink.h"
#include <util/assert.h>

namespace sp {
//=====================================
Sink::Sink(CircularByteBuffer &b, void *a, FlushType s) noexcept
    : buffer(b)
    , sink(s)
    , arg(a) {
}

Sink::~Sink() noexcept {
  flush(*this);
}

//=====================================
static bool
write(Sink &self, const unsigned char *w, std::size_t l) {
  if (l > capacity(self.buffer)) {
    if (self.sink == nullptr) {
      assertx(false);
      return false;
    }

    if (!flush(self)) {
      return false;
    }

    // TODO something like this
    // CircularByteBuffer dummy(w,l)
    // return self.sink(dummy);
    return false;
  } else if (l > remaining_write(self.buffer)) {
    if (!flush(self)) {
      return false;
    }
  }

  if (!write(self.buffer, w, l)) {
    return false;
  }
  return true;
}

bool
write(Sink &self, BytesView &in) noexcept {
  auto length = remaining_read(in);
  if (!write(self, offset(in), length)) {
    return false;
  }
  in.pos += length;
  return true;
}

bool
write(Sink &self, const void *w, std::size_t l) noexcept {
  return write(self, (const unsigned char *)w, l);
}

bool
write(Sink &self, unsigned char c) noexcept {
  return write(self, &c, 1);
}

bool
write(Sink &self, char c) noexcept {
  return write(self, &c, 1);
}

//=====================================
static std::size_t
push_back(Sink &self, const unsigned char *w, std::size_t len) noexcept {
  std::size_t written = 0;

// TODO if buffer is empty and $len > buffer.cap then flush directly fron $w.
Lit:
  written += push_back(self.buffer, w + written, len - written);
  if (written < len) {
    if (flush(self)) {
      goto Lit;
    }
  }
  return written;
}

std::size_t
push_back(Sink &self, const void *w, std::size_t len) noexcept {
  return push_back(self, (const unsigned char *)w, len);
}

bool
push_back(Sink &self, unsigned char c) noexcept {
  return push_back(self, &c, 1);
}

bool
push_back(Sink &self, char c) noexcept {
  return push_back(self, &c, 1);
}

std::size_t
push_back(Sink &self, BytesView &in) noexcept {
  const unsigned char *w = in.raw + in.pos;
  std::size_t written = push_back(self, w, in.length);
  in.pos += written;
  return written;
}

//=====================================
bool
flush(Sink &self) noexcept {
  if (self.sink) {
    return self.sink(self.buffer, self.arg);
  }

  return false;
}

//=====================================
} // namespace sp
