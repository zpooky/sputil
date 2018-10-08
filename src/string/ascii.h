#ifndef SP_UTIL_STRING_UTIL_H
#define SP_UTIL_STRING_UTIL_H

#include <string/view.h>
#include <cstddef>

namespace ascii {
//=====================================
void
reflect(char needle, char *str, std::size_t length) noexcept;

} // namespace ascii

#endif
