#ifndef SP_UTIL_MAP_PROBING_HASH_MAP_H
#define SP_UTIL_MAP_PROBING_HASH_MAP_H

#include <util/maybe.h>
#include <hash/util.h>

namespace sp {
template <typename Key, typename Value, typename H = Hasher<Key>>
struct ProbingHashMap { //
};

} // namespace sp

#endif
