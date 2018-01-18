#ifndef SP_UTIL_HASH_DJB2_H
#define SP_UTIL_HASH_DJB2_H

#include <cstdint>
#include <cstddef>

namespace djb2 {
std::uint32_t
encode(const void *buf, std::size_t length, std::uint32_t hash) noexcept;

std::uint32_t
encode32(const void *buf, std::size_t length) noexcept;

}
namespace djb2a {

std::uint32_t
encode(const void *buf, std::size_t length, std::uint32_t hash) noexcept;

std::uint32_t
encode32(const void *buf, std::size_t length) noexcept;

}

#endif
