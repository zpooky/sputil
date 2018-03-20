#ifndef SP_UTIL_UTIL_SWAP_H
#define SP_UTIL_UTIL_SWAP_H

namespace sp {

template <typename T>
void
swap(T &, T &) noexcept;

template <typename T>
void
swap(T *&f, T *&s) noexcept {
  T *tmp = f;
  f = s;
  s = tmp;
}

//TODO is_triviable copyable = xorshift
//TODO has move constructor = T impl

} // namespace sp

#endif
