#ifndef SP_UTIL_HASH_FNV_H
#define SP_UTIL_HASH_FNV_H

#include <cstdint>
#include <cstddef>

namespace fnv_1a {
std::uint64_t
encode(const void *buf, std::size_t length, std::uint64_t hash) noexcept;

std::uint64_t
encode(const void *buf, std::size_t length) noexcept;
}

#endif
