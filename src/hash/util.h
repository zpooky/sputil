#ifndef SP_UTIL_HASH_UTIL_H
#define SP_UTIL_HASH_UTIL_H

namespace sp {
template <typename T>
using Hasher = std::size_t (*)(const T &);
}

#endif