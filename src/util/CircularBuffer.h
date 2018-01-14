#ifndef SP_UTIL_UTIL_CIRCULAR_BUFFER
#define SP_UTIL_UTIL_CIRCULAR_BUFFER

namespace sp {

/*Circular Buffer overwrites while a Buffer does not*/
template <typename T>
struct CircularBuffer {
  T *buffer;
  std::size_t read;
  std::size_t write;
  const std::size_t capacity;

private:
  CircularBuffer(T *b, std::size_t l) noexcept
      : buffer{b}
      , read{0}
      , write{0}
      , capacity{l} {
  }

public:
  template <std::size_t SIZE>
  explicit CircularBuffer(T (&b)[SIZE]) noexcept
      : CircularBuffer(b, SIZE) {
    static_assert((SIZE & (SIZE - 1)) == 0, "required to be power of 2");
  }

  CircularBuffer(const CircularBuffer &) = delete;
  CircularBuffer(const CircularBuffer &&) = delete;

  CircularBuffer
  operator=(const CircularBuffer &&) = delete;
  CircularBuffer
  operator=(const CircularBuffer &) = delete;
};

namespace impl {
namespace CircularBuffer {

static std::size_t
index(std::size_t in, std::size_t capacity) noexcept {
  return in & (capacity - 1);
}
} // namespace CircularBuffer
} // namespace impl

template <typename T>
std::size_t
length(const CircularBuffer<T> &b) noexcept {
  return b.write - b.read;
}

template <typename T>
bool
is_empty(const CircularBuffer<T> &b) noexcept {
  return b.read == b.write;
}

template <typename T>
bool
is_full(const CircularBuffer<T> &b) noexcept {
  return length(b) == b.capacity;
}

template <typename T, typename I>
T &
push_back(CircularBuffer<T> &b, I &&insert) noexcept {
  if (is_full(b)) {
    b.read++;
  }

  std::size_t idx = impl::CircularBuffer::index(b.write++, b.capacity);
  b.buffer[idx] = std::forward<I>(insert);
  return b.buffer[idx];
}

template <typename T>
bool
pop_front(CircularBuffer<T> &b, T &out) noexcept {
  if (is_empty(b)) {
    return false;
  }

  std::size_t idx = impl::CircularBuffer::index(b.read++, b.capacity);
  out = std::move(b.buffer[idx]);
  return true;
}

} // namespace sp

#endif
