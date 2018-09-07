#include "standard.h"
#include <hash/fnv.h>

namespace sp {
//=====================================
std::size_t
Hasher<bool>::operator()(bool in) const noexcept {
  return fnv_1a::encode32(&in, sizeof(in));
}

//=====================================
std::size_t
Hasher<signed char>::operator()(signed char in) const noexcept {
  return fnv_1a::encode32(&in, sizeof(in));
}

//=====================================
std::size_t
Hasher<unsigned char>::operator()(unsigned char in) const noexcept {
  return fnv_1a::encode32(&in, sizeof(in));
}

//=====================================
std::size_t
Hasher<signed short>::operator()(signed short in) const noexcept {
  return fnv_1a::encode32(&in, sizeof(in));
}

//=====================================
std::size_t
Hasher<unsigned short>::operator()(unsigned short in) const noexcept {
  return fnv_1a::encode32(&in, sizeof(in));
}
//=====================================
std::size_t
Hasher<signed int>::operator()(signed int in) const noexcept {
  return fnv_1a::encode32(&in, sizeof(in));
}

//=====================================
std::size_t
Hasher<unsigned int>::operator()(unsigned int in) const noexcept {
  return fnv_1a::encode32(&in, sizeof(in));
}
//=====================================
std::size_t
Hasher<signed long>::operator()(signed long in) const noexcept {
  return fnv_1a::encode32(&in, sizeof(in));
}

//=====================================
std::size_t
Hasher<unsigned long>::operator()(unsigned long in) const noexcept {
  return fnv_1a::encode32(&in, sizeof(in));
}
}
