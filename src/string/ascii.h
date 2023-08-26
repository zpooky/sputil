#ifndef SP_UTIL_STRING_UTIL_H
#define SP_UTIL_STRING_UTIL_H

#include <cstddef>
#include <string/view.h>

namespace ascii {
//=====================================
void
reflect(char needle, char *str, std::size_t length) noexcept;

//=====================================
bool
is_alpha(const char *, std::size_t) noexcept;

//=====================================
bool
is_printable(char) noexcept;

bool
is_printable(unsigned char) noexcept;

bool
is_printable(const char *, std::size_t) noexcept;

bool
is_printable(const unsigned char *, std::size_t) noexcept;

//=====================================
} // namespace ascii

#endif
