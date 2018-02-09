#ifndef SP_UTIL_TRIE_TRIE_H
#define SP_UTIL_TRIE_TRIE_H

namespace sp {
namespace impl {
namespace trie {
struct TrieNode {
};

}
}

template<typename Value>
struct Trie {
};

template<typename Value, typename V>
Value* insert(Trie<Value>&, V&&) noexcept;

/*
 * # find exact match
 * - find()
 * # remove exact match
 * - remove()
 * # replace exact match with another value
 * - replace()
 * # remove all matching prefix
 * - remove_prefix()
 */

/*
 * ==========================================================================
 */
}

#endif
