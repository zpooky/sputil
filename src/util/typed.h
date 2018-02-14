#ifndef SP_UTIL_UTIL_TYPED_H
#define SP_UTIL_UTIL_TYPED_H

#include <cstddef>

#define SIZE_TYPE(NAME)                                                        \
  struct NAME {                                                                \
    std::size_t data;                                                          \
    explicit constexpr NAME(std::size_t d) noexcept                            \
        : data(d) {                                                            \
    }                                                                          \
    constexpr NAME() noexcept                                                  \
        : NAME(std::size_t(0)) {                                               \
    }                                                                          \
    constexpr bool                                                             \
    operator==(std::size_t o) const noexcept {                                 \
      return data == o;                                                        \
    }                                                                          \
    constexpr bool                                                             \
    operator==(const NAME &o) const noexcept {                                 \
      return this->operator==(o.data);                                         \
    }                                                                          \
    constexpr bool                                                             \
    operator!=(std::size_t o) const noexcept {                                 \
      return !operator==(o);                                                   \
    }                                                                          \
    constexpr NAME &                                                           \
    operator=(std::size_t o) noexcept {                                        \
      data = o;                                                                \
      return *this;                                                            \
    }                                                                          \
    constexpr NAME                                                             \
    operator+(std::size_t o) const noexcept {                                  \
      return NAME{data + o};                                                   \
    }                                                                          \
    constexpr NAME                                                             \
    operator+(const NAME &o) const noexcept {                                  \
      return operator+(o.data);                                                \
    }                                                                          \
    constexpr NAME                                                             \
    operator-(std::size_t o) const noexcept {                                  \
      return NAME{data - o};                                                   \
    }                                                                          \
    constexpr NAME                                                             \
    operator-(NAME o) const noexcept {                                         \
      return operator-(o.data);                                                \
    }                                                                          \
    constexpr bool                                                             \
    operator>(std::size_t o) const noexcept {                                  \
      return data > o;                                                         \
    }                                                                          \
    constexpr bool                                                             \
    operator>(const NAME &o) const noexcept {                                  \
      return operator>(o.data);                                                \
    }                                                                          \
    constexpr bool                                                             \
    operator>=(std::size_t o) const noexcept {                                 \
      return data >= o;                                                        \
    }                                                                          \
    constexpr bool                                                             \
    operator>=(const NAME &o) const noexcept {                                 \
      return operator>=(o.data);                                               \
    }                                                                          \
    constexpr bool                                                             \
    operator<(std::size_t o) const noexcept {                                  \
      return data < o;                                                         \
    }                                                                          \
    constexpr bool                                                             \
    operator<(const NAME &o) const noexcept {                                  \
      return operator<(o.data);                                                \
    }                                                                          \
    constexpr bool                                                             \
    operator<=(std::size_t o) const noexcept {                                 \
      return data <= o;                                                        \
    }                                                                          \
    constexpr bool                                                             \
    operator<=(const NAME &o) const noexcept {                                 \
      return operator<=(o.data);                                               \
    }                                                                          \
    constexpr NAME                                                             \
    operator/(std::size_t o) const noexcept {                                  \
      return NAME{data / o};                                                   \
    }                                                                          \
    constexpr NAME                                                             \
    operator%(std::size_t o) const noexcept {                                  \
      return NAME{data % o};                                                   \
    }                                                                          \
    constexpr NAME operator*(std::size_t o) const noexcept {                   \
      return NAME{data * o};                                                   \
    }                                                                          \
    constexpr explicit operator std::size_t() const noexcept {                 \
      return data;                                                             \
    }                                                                          \
  };                                                                           \
  static_assert(sizeof(NAME) == sizeof(std::size_t), "");                      \
  static_assert(alignof(NAME) == alignof(std::size_t), "")

// example:
// SIZE_TYPE(bucket_size);

#endif
