#ifndef SP_UTIL_ENCODE_HEX_H
#define SP_UTIL_ENCODE_HEX_H

#include <cstddef>
#include <cstdint>

namespace hex {
bool
decode(const char *, /*OUT*/ std::uint8_t *, /*IN/OUT*/ std::size_t &) noexcept;

bool
encode(const std::uint8_t *, /*OUT*/ char *, /*IN/OUT*/ std::size_t &) noexcept;
} // namespace hex
#endif
