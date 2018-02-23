#include "gtest/gtest.h"
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

#define assert_elements(node, init)                                            \
  do {                                                                         \
    ASSERT_TRUE(node);                                                         \
    auto l = init;                                                             \
    ASSERT_EQ(l.size(), node->elements.length);                                \
    std::size_t i = 0;                                                         \
    for (auto &c : l) {                                                        \
      auto elem = get(node->elements, i++);                                    \
      ASSERT_TRUE(elem);                                                       \
      ASSERT_EQ(c, *elem);                                                     \
    }                                                                          \
  } while (0)

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
    assert_elements(t.root, std::initializer_list<int>{1});
    assert_zero_children(t.root);
  }
  {
    assert_insert(t, 2);
    assert_elements(t.root, std::initializer_list<int>({1, 2}));
    assert_zero_children(t.root);
  }
  {
    assert_insert(t, 3);
    assert_elements(t.root, std::initializer_list<int>({2}));
    assert_child(t.root, 0, std::initializer_list<int>{1});
    assert_child(t.root, 1, std::initializer_list<int>{3});
  }
}
