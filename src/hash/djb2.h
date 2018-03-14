#ifndef SP_UTIL_HASH_DJB2_H
#define SP_UTIL_HASH_DJB2_H

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace djb2 {
std::uint32_t
encode(const void *buf, std::size_t length, std::uint32_t hash) noexcept;

std::uint32_t
encode32(const void *buf, std::size_t length) noexcept;

} // namespace djb2
namespace djb2a {

std::uint32_t
encode(const void *buf, std::size_t length, std::uint32_t hash) noexcept;

std::uint32_t
encode32(const void *buf, std::size_t length) noexcept;

template <typename T>
std::enable_if_t<std::is_trivially_copyable<T>::value, std::size_t>
hash(const T &in) noexcept {
  return encode32(&in, sizeof(in));
}

} // namespace djb2a

#endif
