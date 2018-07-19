#ifndef SP_UTIL_STRING_LEVENSHTEIN_H
#define SP_UTIL_STRING_LEVENSHTEIN_H

#include <cstdint>

namespace ascii {
std::size_t
levenshtein(const char *, const char *) noexcept;

std::size_t
levenshtein2(const char *, const char *) noexcept;

void
p();

namespace dp {
std::size_t
levenshtein(const char *, const char *) noexcept;
}
}

#endif
