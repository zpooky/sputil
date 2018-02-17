#include "Sink.h"

namespace sp {

Sink::Sink(ByteBuffer &b) noexcept
    : buffer(b)
    , sink(nullptr) {
}

std::size_t
write(Sink &, const unsigned char *, std::size_t) noexcept {
  return 0;
}

bool
flush(Sink &) noexcept {
  return true;
}

} // namespace sp
