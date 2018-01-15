#ifndef SP_UTIL_HASH_CRC_H
#define SP_UTIL_HASH_CRC_H

#include <cstdint>

namespace crc32c {

std::uint32_t
encode(const void *buffer, std::size_t length) noexcept;

}

namespace crc32 {

std::uint32_t
encode(const void *buffer, std::size_t length) noexcept;
}

#endif
