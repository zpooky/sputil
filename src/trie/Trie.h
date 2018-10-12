#ifndef SP_UTIL_TRIE_TRIE_H
#define SP_UTIL_TRIE_TRIE_H

#include <list/LinkedList.h>

// TODO
namespace sp {
//=====================================
namespace impl {
template <typename Value>
struct TrieNode {
  sp::LinkedList<TrieNode> children;

  TrieNode() noexcept;

  TrieNode(const TrieNode &) = delete;
  TrieNode(const TrieNode &&) = delete;

  ~TrieNode() noexcept;
};
}

//=====================================
template <typename Value>
struct Trie {
  impl::TrieNode<Value> root;

  Trie() noexcept;

  Trie(const Trie &) = delete;
  Trie(const Trie &&) = delete;

  ~Trie() noexcept;
};

//=====================================
template <typename Value, typename V>
Value *
insert(Trie<Value> &, const char *, V &&) noexcept;

//=====================================
template <typename Value>
const Value *
lookup(const Trie<Value> &, const char *) noexcept;

template <typename Value>
Value *
lookup(Trie<Value> &, const char *) noexcept;

//=====================================
template <typename Value>
bool
remove(Trie<Value> &, const char *) noexcept;

//=====================================
template <typename Value>
std::size_t
remove_prefix(Trie<Value> &, const char *) noexcept;

//=====================================
template <typename Value, typename F>
void
for_each_prefix(Trie<Value> &, const char *prefix, F) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace impl {
template <typename Value>
TrieNode<Value>::TrieNode() noexcept
    : children{} {
}

template <typename Value>
TrieNode<Value>::~TrieNode() noexcept {
}
}

//=====================================
template <typename Value>
Trie<Value>::Trie() noexcept
    : root{} {
}

template <typename Value>
Trie<Value>::~Trie() noexcept {
}

//=====================================
}

#endif
