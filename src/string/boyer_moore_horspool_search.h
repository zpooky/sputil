#ifndef SP_UTIL_STRING_BOYER_MOORE_HORSPOOL_SEARCH_H
#define SP_UTIL_STRING_BOYER_MOORE_HORSPOOL_SEARCH_H

#include <cstddef>

namespace sp {
namespace bmh {
//=====================================
const char *
search(const char *text, std::size_t, const char *needle, std::size_t) noexcept;

const char *
search(const char *text, std::size_t, const char *needle) noexcept;

const char *
search(const char *text, const char *needle) noexcept;

//=====================================
}
}

#endif
