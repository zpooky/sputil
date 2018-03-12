#include "gtest/gtest.h"
#include <cassert>
#include <initializer_list>
#include <tree/btree.h>

#define assert_insert(t, v)                                                    \
  do {                                                                         \
    printf("# %d\n", v);                                                       \
    { /*test find*/                                                            \
      printf("1. find non\n");                                                 \
      auto *res = find(t, v);                                                  \
      ASSERT_FALSE(res);                                                       \
    }                                                                          \
    { /*insert*/                                                               \
      printf("2. insert\n");                                                   \
      auto r = insert(t, v);                                                   \
      dump(t);                                                                 \
      auto *res = std::get<0>(r);                                              \
      { /*test insert*/                                                        \
        auto inserted = std::get<1>(r);                                        \
        ASSERT_TRUE(inserted);                                                 \
        ASSERT_TRUE(res);                                                      \
        ASSERT_EQ(*res, v);                                                    \
      }                                                                        \
      { /*test duplicate insert*/                                              \
        printf("3. insert dup\n");                                             \
        auto dupr = insert(t, v);                                              \
        auto inserted = std::get<1>(dupr);                                     \
        if (inserted)                                                          \
          dump(t);                                                             \
        ASSERT_FALSE(inserted);                                                \
        auto *dupres = std::get<0>(dupr);                                      \
        ASSERT_TRUE(res);                                                      \
        ASSERT_EQ(dupres, res);                                                \
        ASSERT_EQ(*dupres, v);                                                 \
      }                                                                        \
    }                                                                          \
    { /*test find*/                                                            \
      printf("4. find exist\n");                                               \
      auto *res = find(t, v);                                                  \
      ASSERT_TRUE(res);                                                        \
      ASSERT_EQ(*res, v);                                                      \
    }                                                                          \
  } while (0)

template <std::size_t keys, std::size_t N>
static void
assert_elements(btree::impl::btree::BTNode<int, keys> *node,
                const int (&elems)[N]) {
  assert(N == node->elements.length);
  for (std::size_t i = 0; i < N; ++i) {
    auto elem = get(node->elements, i);
    assert(elem);
    assert(elems[i] == *elem);
  }
}

template <std::size_t keys, std::size_t N>
static bool
sp_assert_child(btree::impl::btree::BTNode<int, keys> *root, std::size_t idx,
                const int (&elems)[N]) {
  auto node = root->children[idx];

  assert_elements(node, elems);
  return true;
}

#define assert_zero_children(node)                                             \
  do {                                                                         \
    ASSERT_TRUE(node);                                                         \
    ASSERT_TRUE(for_all(node->children, [](const auto &n) {                    \
      /**/                                                                     \
      return n == nullptr;                                                     \
    }));                                                                       \
  } while (0)

#define assert_child(node, idx, init)                                          \
  do {                                                                         \
    ASSERT_TRUE(node);                                                         \
    auto child = get(node->children, idx);                                     \
    ASSERT_TRUE(child);                                                        \
    assert_elements((*child), init);                                           \
  } while (0)

TEST(btreeTest, test_shape) {
  constexpr std::size_t order = 3;
  constexpr std::size_t keys = order - 1;
  btree::Tree<int, keys> t;
  {
    assert_insert(t, 1);
    assert_elements(t.root, {1});
    assert_zero_children(t.root);
  }
  {
    assert_insert(t, 2);
    assert_elements(t.root, {1, 2});
    assert_zero_children(t.root);
  }
  {
    assert_insert(t, 3);
    assert_elements(t.root, {2});
    sp_assert_child(t.root, 0, {1});
    sp_assert_child(t.root, 1, {3});
  }
  {
    assert_insert(t, 4);
    assert_elements(t.root, {2});
    sp_assert_child(t.root, 0, {1});
    sp_assert_child(t.root, 1, {3, 4});
  }
  // {
  //   assert_insert(t, 5);
  //   assert_elements(t.root, {2, 4});
  //   sp_assert_child(t.root, 0, {1});
  //   sp_assert_child(t.root, 1, {3});
  //   sp_assert_child(t.root, 2, {5});
  // }
}
