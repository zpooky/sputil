#ifndef SP_UTIL_HASH_FNV_H
#define SP_UTIL_HASH_FNV_H

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace fnv_1a {
std::uint64_t
encode(const void *buf, std::size_t length, std::uint64_t hash) noexcept;

std::uint64_t
encode64(const void *buf, std::size_t length) noexcept;

std::uint32_t
encode(const void *buf, std::size_t length, std::uint32_t hash) noexcept;

std::uint32_t
encode32(const void *buf, std::size_t length) noexcept;

template <typename T>
std::enable_if_t<std::is_trivially_copyable<T>::value, std::size_t>
hash(const T &in) noexcept {
  return encode32(&in, sizeof(in));
}
} // namespace fnv_1a

#endif
