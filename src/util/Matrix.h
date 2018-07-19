#ifndef SP_UTIL_UTIL_MATRIX_H
#define SP_UTIL_UTIL_MATRIX_H

#include <cstddef>
#include <util/assert.h>

namespace sp {

template <typename T>
struct Matrix {
  T *buffer;
  const std::size_t height;
  const std::size_t width;

  Matrix(std::size_t y, std::size_t x) noexcept
      : buffer{nullptr}
      , height{y}
      , width{x} {
    buffer = new T[height * width]{};
  }

  ~Matrix() noexcept {
    if (buffer) {
      delete[] buffer;
    }
    buffer = nullptr;
  }

  const T *operator[](std::size_t y) const noexcept {
    assertx(buffer);
    assertxs(y < height, y, height);
    return buffer + (y * width);
  }

  T *operator[](std::size_t y) noexcept {
    const Matrix<T> *ct = this;
    return (T *)ct->operator[](y);
  }
};

template <typename T, typename F>
void
for_each(Matrix<T> &m, F f) noexcept {
  for (std::size_t y = 0; y < m.height; ++y) {
    for (std::size_t x = 0; x < m.width; ++x) {
      f(y, x, m[y][x]);
    }
  }
}

template<typename T>
void
print(const Matrix<T> &) noexcept;
}

#endif
