#ifndef SP_UTIL_STRING_LEVENSHTEIN_H
#define SP_UTIL_STRING_LEVENSHTEIN_H

#include <cstdint>

namespace ascii {
std::uint32_t
levenshtein(const char *, const char *) noexcept;
}

#endif
