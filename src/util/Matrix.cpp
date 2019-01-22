#include "Matrix.h"
#include <cstdio>

namespace sp {
//=====================================
static std::size_t
max(std::size_t f, std::size_t s) noexcept {
  return f > s ? f : s;
}

template <typename T>
static std::size_t
padding(T in) noexcept {
  if (in > 9) {
    if (in > 99) {
      if (in > 999) {
        if (in > 9999) {
          if (in > 99999) {
            if (in > 999999) {
              if (in > 9999999) {
                return 8;
              }
              return 7;
            }
            return 6;
          }
          return 5;
        }
        return 4;
      }
      return 3;
    }
    return 2;
  }
  return 1;
}

template <typename T>
static std::size_t
padding(const Matrix<T> &m, std::size_t x) noexcept {
  std::size_t result = 0;
  for (std::size_t y = 0; y < m.height; ++y) {
    result = max(result, padding(m[y][x]));
  }

  return result;
}

static void
do_print(std::size_t value) noexcept {
  printf("%zu", value);
}

static void
do_print(ssize_t value) noexcept {
  if (value < 0) {
    printf("-");
  } else {
    printf("%zd", value);
  }
}

static void
do_print(char value) noexcept {
  printf("%c", value);
}

static void
do_print(unsigned char value) noexcept {
  printf("%u", value);
}

#ifdef __x86_64
static void
do_print(int value) noexcept {
  printf("%02d", value);
}
#endif

template <typename T>
static void
print(T value, std::size_t pad) noexcept {
  std::size_t p = padding(value);
  for (std::size_t i = 0; i < pad - p; ++i) {
    printf(" ");
  }
  do_print(value);
}

//=====================================
template <typename T>
void
print(const Matrix<T> &m) noexcept {
  printf("_________________________________________\n");
  for (std::size_t y = 0; y < m.height; ++y) {
    for (std::size_t x = 0; x < m.width; ++x) {
      std::size_t p = padding(m, x);
      print(m[y][x], p);
      printf(" ");
    }
    printf("\n");
  }
}

/* Explicit template instantiation
 */
template void
print<std::size_t>(const Matrix<std::size_t> &) noexcept;

template void
print<ssize_t>(const Matrix<ssize_t> &) noexcept;

template void
print<int>(const Matrix<int> &) noexcept;

template void
print<char>(const Matrix<char> &) noexcept;

template void
print<unsigned char>(const Matrix<unsigned char> &) noexcept;

//=====================================
} // namespace sp
