#ifndef SP_UTIL_HASH_UTIL_H
#define SP_UTIL_HASH_UTIL_H

#include <cstddef>

namespace sp {
template <typename T>
using hasher = std::size_t (*)(const T &);
}

#endif
