#ifndef SP_UTIL_UTIL_SWAP_H
#define SP_UTIL_UTIL_SWAP_H

namespace sp {

template <typename T>
void
swap(T &, T &) noexcept;

// template <>
template <typename T>
void
swap(T *&f, T *&s) noexcept {
  T *tmp = f;
  f = s;
  s = tmp;
}

// std::enable_if_t<std::is_trivially_copyable<T>::value, void>
// swap

// TODO is_triviable copyable = xorshift
// TODO has move constructor = T impl

} // namespace sp

#endif
