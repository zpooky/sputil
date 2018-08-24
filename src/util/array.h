#ifndef SP_UTIL_UTIL_ARRAY_H
#define SP_UTIL_UTIL_ARRAY_H

namespace sp {
//=====================================
template <typename T>
std::size_t
index_of(const T *arr, std::size_t length, std::size_t capacity,
         const T *needle) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T>
std::size_t
index_of(const T *const arr, std::size_t l, std::size_t cap,
         const T *ptr) noexcept {
  assertxs(arr, l, cap);
  assertxs(ptr, l, cap);

  if (ptr) {
    auto s = reinterpret_cast<std::uintptr_t>(arr);
    auto e = reinterpret_cast<std::uintptr_t>(ptr);

    if (e >= s) {
      auto index = e - s;
      // assertx(index % sizeof(*ptr) == 0);??
      index /= sizeof(*ptr);

      if (index < l) {
        return index;
      }
    }
  }

  return cap;
}
}

#endif
