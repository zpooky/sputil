#ifndef SP_UTIL_BUFFER_SINK_H
#define SP_UTIL_BUFFER_SINK_H

#include <buffer/BytesView.h>
#include <buffer/CircularByteBuffer.h>
// TODO impl

namespace sp {
//=====================================
struct Sink {
  using FlushType = bool (*)(CircularByteBuffer &, void *);

  CircularByteBuffer &buffer;
  FlushType sink;
  void *arg;

  Sink(CircularByteBuffer &b, void *a, FlushType s) noexcept;
  Sink(const Sink &) = delete;
  Sink(const Sink &&) = delete;

  Sink &
  operator=(const Sink &) = delete;
  Sink &
  operator=(const Sink &&) = delete;

  ~Sink() noexcept;
};

//=====================================
/* write all or nothing */
bool
write(Sink &, BytesView &) noexcept;

bool
write(Sink &, const void *, std::size_t) noexcept;

bool
write(Sink &, unsigned char) noexcept;

bool
write(Sink &, char) noexcept;

template <std::size_t SIZE>
bool
write(Sink &s, const unsigned char (&buffer)[SIZE]) noexcept {
  return write(s, buffer, SIZE);
}

template <std::size_t SIZE>
bool
write(Sink &s, const char (&buffer)[SIZE]) noexcept {
  return write(s, buffer, SIZE);
}

//=====================================
/* writes as much as possible & returns bytes written */
std::size_t
push_back(Sink &, const void *, std::size_t) noexcept;

bool
push_back(Sink &, unsigned char) noexcept;

bool
push_back(Sink &, char) noexcept;

template <std::size_t SIZE>
std::size_t
push_back(Sink &s, const unsigned char (&buffer)[SIZE]) noexcept {
  return push_back(s, buffer, SIZE);
}

std::size_t
push_back(Sink &, BytesView &) noexcept;

//=====================================
bool
flush(Sink &) noexcept;

//=====================================
} // namespace sp
#endif
