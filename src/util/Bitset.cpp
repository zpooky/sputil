#include "Bitset.h"
#include <cstdlib>
#include <tree/bst.h>
#include <type_traits>
#include <util/assert.h>

namespace sp {
struct Bitset_buffer {
  using type = std::remove_reference<
      std::remove_pointer<decltype(Bitset::buffer)>::type>::type;
};

static std::size_t
capacity(const SparseBitset &b) noexcept {
  return b.blocks * b.block_size;
}

static std::size_t
block_index(const SparseBitset &b, std::size_t abs_idx) noexcept {
  std::size_t block_bits(b.block_size * sizeof(Bitset_buffer::type) * 8);
  return abs_idx / block_bits;
}

static std::size_t
word_index(std::size_t abs_idx) noexcept {
  constexpr std::size_t bits(sizeof(Bitset_buffer::type) * 8);
  return abs_idx / bits;
}

static std::size_t
bit_index(std::size_t abs_idx) noexcept {
  constexpr std::size_t bits(sizeof(Bitset_buffer::type) * 8);
  return abs_idx % bits;
}

static Bitset_buffer::type
mask_out(std::size_t idx) noexcept {
  typename Bitset_buffer::type result = 0;
  return (result | 1) << idx;
}

static bool
test(typename Bitset_buffer::type word, std::size_t bit_idx) noexcept {
  auto mask = mask_out(bit_idx);
  return (mask & word) == mask;
}

static void
set(typename Bitset_buffer::type &word, std::size_t bit_idx, bool v) noexcept {
  const auto mask = mask_out(bit_idx);
  if (v) {
    word = word | mask;
  } else {
    word = word & (~mask);
  }
}

static std::uint64_t *
sparse_start(void *in) noexcept {
  assertx(in);
  void *start = (unsigned char *)in;
  assertx(reinterpret_cast<uintptr_t>(start) % alignof(std::uint64_t) == 0);
  return (std::uint64_t *)start;
}

static std::uint64_t *
sparse_buffer(void *in) noexcept {
  void *start = sparse_start(in);
  void *buffer = ((unsigned char *)start) + sizeof(std::uint64_t);
  assertx(reinterpret_cast<uintptr_t>(buffer) % alignof(std::uint64_t *) == 0);
  return (std::uint64_t *)buffer;
}

struct SparseEntry {
  void *raw;
  SparseEntry(void *r) noexcept
      : raw(r) {
    assertx(raw);
  }

  SparseEntry(const SparseEntry &) = delete;
  SparseEntry(SparseEntry &&o) noexcept
      : raw(o.raw) {
    o.raw = nullptr;
  }

  SparseEntry &
  operator=(const SparseEntry &) = delete;
  SparseEntry &
  operator=(const SparseEntry &&) = delete;

  ~SparseEntry() noexcept {
    if (raw) {
      free(raw);
      raw = nullptr;
    }
  }

  std::uint64_t *
  bitset() noexcept {
    return sparse_buffer(raw);
  }

  std::uint64_t *
  bitset() const noexcept {
    return sparse_buffer(raw);
  }

  std::uint64_t
  start() const noexcept {
    return *sparse_start(raw);
  }
};
using SBitsetTreeType = binary::Tree<SparseEntry>;
using SBitsetNodeType = binary::Node<SparseEntry>;

static bool
operator>(const SparseEntry &f, std::size_t s) noexcept {
  return f.start() > s;
}

static bool
operator>(const SparseEntry &f, const SparseEntry &s) noexcept {
  return f.start() > s.start();
}

static bool
operator>(std::size_t f, const SparseEntry &s) noexcept {
  return f > s.start();
}

template <typename F>
static auto
with_tree(SparseBitset &b, F f) noexcept {
  SBitsetTreeType tree((SBitsetNodeType *)b.tree);

  auto result = f(tree);

  b.tree = tree.root;
  tree.root = nullptr;
  return result;
}

template <typename F>
static auto
with_tree(const SparseBitset &b, F f) noexcept {
  SBitsetTreeType tree((SBitsetNodeType *)b.tree);

  auto result = f((const SBitsetTreeType &)tree);

  tree.root = nullptr;
  return result;
}

static const SparseEntry *
block_for(const SparseBitset &b, std::size_t index) noexcept {
  if (b.tree) {
    return with_tree(b, [&](const auto &tree) -> const SparseEntry * { //
      return find(tree, index);
    });
  }

  return nullptr;
}

static SparseEntry *
block_for(SparseBitset &b, std::size_t index) noexcept {
  const SparseBitset &c_b = b;
  return (SparseEntry *)block_for(c_b, index);
}

static void *
alloc_block(const SparseBitset &b, std::size_t start) noexcept {
  // XXX
  constexpr std::size_t alignment = alignof(std::uint64_t);
  // constexpr std::size_t alignment =
  // max_alignment< td::size_t, typename Bitset_buffer::type>::value;

  std::size_t block_bytes = b.block_size * sizeof(Bitset_buffer::type);
  std::size_t size = sizeof(std::uint64_t) + block_bytes;

  void *const raw = aligned_alloc(alignment, size);
  if (raw) {
    ::new (sparse_start(raw)) std::uint64_t(start);
    ::new (sparse_buffer(raw)) std::uint64_t[b.block_size]{0};
  }
  return raw;
}

static SparseEntry *
block_for_insert(SparseBitset &b, std::size_t index) noexcept {
  // XXX find_or_insert(b,index,[]{});
  SparseEntry *result = block_for(b, index);
  if (!result) {
    void *mem = alloc_block(b, index);
    if (mem) {
      return with_tree(b, [&](auto &tree) -> SparseEntry * {
        auto insres = insert(tree, SparseEntry(mem));
        SparseEntry *const inserted = std::get<0>(insres);
        if (inserted) {
          assertx(inserted->start() == index);
          // assert allocated new node in tree
          assertx(std::get<1>(insres));

          return inserted;
        }
        assertx(false);

        return nullptr;
      });
    }
  }

  return result;
}

//=====================================
//====Implementation===================
//=====================================
Bitset::Bitset(std::uint64_t *b, std::size_t c) noexcept
    : buffer{b}
    , capacity{c} {
}

SparseBitset::SparseBitset(std::size_t bs, std::size_t c) noexcept
    : block_size(bs)
    , blocks(c)
    , tree(nullptr) {
}

SparseBitset::~SparseBitset() noexcept {
  if (tree) {
    SBitsetTreeType((SBitsetNodeType *)tree);
  }
}

//=====================================
bool
test(const Bitset &b, std::size_t idx) noexcept {
  std::size_t wIdx = word_index(idx);
  assertx(wIdx < b.capacity);

  std::size_t bIdx = bit_index(idx);
  auto word = b.buffer[wIdx];

  return test(word, bIdx);
}

bool
test(const SparseBitset &b, std::size_t abs_idx) noexcept {
  assertx(abs_idx < bits(b));

  std::size_t block_idx = block_index(b, abs_idx);
  // printf("block_for(b,block_idx[%zu])\n", block_idx);
  const SparseEntry *block = block_for(b, block_idx);
  if (block) {
    Bitset bblock(block->bitset(), b.block_size);
    std::size_t bits_offset = block->start() * bits(bblock);
    std::size_t idx(abs_idx - bits_offset);
    return test(bblock, idx);
  }

  return false;
}

//=====================================
bool
set(Bitset &b, std::size_t idx, bool v) noexcept {
  std::size_t wIdx = word_index(idx);
  assertxs(wIdx < b.capacity, wIdx, b.capacity);

  auto &word = b.buffer[wIdx];
  const auto old_word = word;

  const std::size_t bIdx = bit_index(idx);
  set(word, bIdx, v);

  return test(old_word, bIdx);
}

bool
set(SparseBitset &b, std::size_t abs_idx, bool v) noexcept {
  assertxs(abs_idx < bits(b), abs_idx, bits(b));

  std::size_t block_idx = block_index(b, abs_idx);
  // printf("block_for_insert(b,block_idx[%zu])\n", block_idx);
  SparseEntry *const block = block_for_insert(b, block_idx);
  if (block) {

    Bitset bblock(block->bitset(), b.block_size);
    std::size_t bits_offset = block->start() * bits(bblock);
    std::size_t idx(abs_idx - bits_offset);
    return set(bblock, idx, v);
  }
  // XXX How to handle failed alloc?
  assertx(false);

  return false;
}

//=====================================
bool
toggle(Bitset &b, std::size_t idx) noexcept {
  std::size_t wIdx = word_index(idx);
  assertxs(wIdx < b.capacity, wIdx, b.capacity);

  const std::size_t bIdx = bit_index(idx);
  auto &word = b.buffer[wIdx];

  const bool v = !test(word, bIdx);
  set(word, bIdx, v);

  return v;
}

bool
toggle(SparseBitset &b, std::size_t abs_idx) noexcept {
  assertxs(abs_idx < bits(b), abs_idx, bits(b));

  std::size_t block_idx = block_index(b, abs_idx);
  SparseEntry *const block = block_for_insert(b, block_idx);
  if (block) {
    Bitset bblock(block->bitset(), b.block_size);
    std::size_t bits_offset = block->start() * bits(bblock);
    std::size_t idx(abs_idx - bits_offset);
    return toggle(bblock, idx);
  }

  // XXX How to handle failed alloc?
  assertx(false);

  return false;
}

//=====================================
static std::size_t
bits(std::size_t capacity) noexcept {
  return sizeof(Bitset_buffer::type) * 8 * capacity;
}

std::size_t
bits(const Bitset &b) noexcept {
  return bits(b.capacity);
}

std::size_t
bits(const SparseBitset &b) noexcept {
  return bits(capacity(b));
}

//=====================================
}
