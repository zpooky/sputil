#include "gtest/gtest.h"
#include <collection/Array.h>
#include <list/LinkedList.h>
#include <memory/StackPooledAllocator.h>
#include <prng/util.h>
#include <prng/xorshift.h>

#define LinkedList_Test(l)                                                     \
  do {                                                                         \
    using vt = typename decltype(l)::value_type;                               \
    {                                                                          \
      ASSERT_FALSE(get(l, 0));                                                 \
      ASSERT_FALSE(find_first(l, [](auto) {                                    \
        assert(false);                                                         \
        return false;                                                          \
      }));                                                                     \
      {                                                                        \
        auto *r = push_back(l, 1);                                             \
        ASSERT_TRUE(r);                                                        \
        ASSERT_EQ(*r, vt(1));                                                  \
      }                                                                        \
      ASSERT_FALSE(get(l, std::size_t(1)));                                    \
      {                                                                        \
        auto *res = find_first(l, [](auto in) {                                \
          /**/                                                                 \
          return in == 1;                                                      \
        });                                                                    \
        ASSERT_TRUE(res);                                                      \
        ASSERT_EQ(*res, vt(1));                                                \
      }                                                                        \
      {                                                                        \
        auto *r = get(l, 0);                                                   \
        ASSERT_TRUE(r);                                                        \
        ASSERT_EQ(*r, vt(1));                                                  \
        for_each(l, [](auto in) {                                              \
          /**/                                                                 \
          assert(in == vt(1));                                                 \
        });                                                                    \
        ASSERT_TRUE(for_all(l, [](auto in) {                                   \
          /**/                                                                 \
          return in == 1;                                                      \
        }));                                                                   \
      }                                                                        \
      ASSERT_FALSE(get(l, 1));                                                 \
      remove_first(l, [](auto in) {                                            \
        /**/                                                                   \
        return in == vt(1);                                                    \
      });                                                                      \
      ASSERT_FALSE(get(l, 0));                                                 \
      ASSERT_TRUE(for_all(l, [](auto) {                                        \
        /**/                                                                   \
        return false;                                                          \
      }));                                                                     \
      clear(l);                                                                \
      ASSERT_FALSE(get(l, 0));                                                 \
      ASSERT_TRUE(for_all(l, [](auto) {                                        \
        /**/                                                                   \
        return false;                                                          \
      }));                                                                     \
    }                                                                          \
    const int max = 100;                                                       \
    for (int i = 0; i < max; ++i) {                                            \
      for (int a = 0; a < i; ++a) {                                            \
        auto *res = get(l, std::size_t(a));                                    \
        ASSERT_TRUE(res);                                                      \
        ASSERT_EQ(*res, a);                                                    \
        {                                                                      \
          auto *res = find_first(l, [&a](auto in) {                            \
            /**/                                                               \
            return in == vt(a);                                                \
          });                                                                  \
          ASSERT_TRUE(res);                                                    \
          ASSERT_EQ(*res, vt(a));                                              \
        }                                                                      \
      }                                                                        \
      auto *res = insert(l, vt(i));                                            \
      ASSERT_TRUE(res);                                                        \
      ASSERT_EQ(*res, vt(i));                                                  \
      for (int a = i + 1; a < max; ++a) {                                      \
        auto *res = get(l, std::size_t(a));                                    \
        ASSERT_FALSE(res);                                                     \
        {                                                                      \
          ASSERT_FALSE(find_first(l, [&a](auto in) {                           \
            /**/                                                               \
            return in == vt(a);                                                \
          }));                                                                 \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    {                                                                          \
      int i = 0;                                                               \
      for_each(l, [&i](auto in) {                                              \
        vt cmp(i);                                                             \
        ++i;                                                                   \
        assert(in == cmp);                                                     \
      });                                                                      \
      ASSERT_EQ(i, max);                                                       \
    }                                                                          \
    {                                                                          \
      int i = 0;                                                               \
      ASSERT_TRUE(for_all(l, [&](auto in) {                                    \
        vt cmp(i);                                                             \
        ++i;                                                                   \
        return in == cmp;                                                      \
      }));                                                                     \
    }                                                                          \
    prng::xorshift32 r(1);                                                     \
    sp::StaticArray<vt, max> in;                                               \
    ASSERT_EQ(in.capacity, std::size_t(max));                                  \
    for (int i = 0; i < max; ++i) {                                            \
      ASSERT_TRUE(insert(in, vt(i)));                                          \
    }                                                                          \
    ASSERT_EQ(in.length, std::size_t(max));                                    \
    shuffle(r, in);                                                            \
                                                                               \
    for (std::size_t i = 0; i < in.length; ++i) {                              \
      for (std::size_t a = 0; a < i; ++a) {                                    \
        ASSERT_FALSE(find_first(l, [&in, a](auto c) {                          \
          /**/                                                                 \
          return c == *get(in, a);                                             \
        }));                                                                   \
      }                                                                        \
                                                                               \
      ASSERT_TRUE(remove_first(l, [&in, i](auto c) {                           \
        /**/                                                                   \
        return c == *get(in, i);                                               \
      }));                                                                     \
                                                                               \
      for (int a = i + 1; a < max; ++a) {                                      \
        auto *res = find_first(l, [&in, a](auto c) {                           \
          /**/                                                                 \
          return c == *get(in, a);                                             \
        });                                                                    \
        ASSERT_TRUE(res);                                                      \
        ASSERT_EQ(*res, *get(in, a));                                          \
      }                                                                        \
    }                                                                          \
  } while (0)

TEST(LinkedListTest, test_ins) {
  sp::LinkedList<int> l;
  LinkedList_Test(l);
}

TEST(LinkedListTest, test_ins_stack_alloc) {
  sp::LinkedList<std::uint64_t, sp::StackPooledAllocator> l;
  LinkedList_Test(l);
}

#define LinkedList_Test_speed(l)                                               \
  do {                                                                         \
    using vt = typename decltype(l)::value_type;                               \
    constexpr vt max = 1000;                                                   \
    for (std::size_t r = 0; r < 500; ++r) {                                    \
      for (vt i = 0; i < max; ++i) {                                           \
        vt *res = insert(l, i);                                                \
        ASSERT_TRUE(res);                                                      \
        ASSERT_EQ(*res, i);                                                    \
      }                                                                        \
      for (vt i = 0; i < max; ++i) {                                           \
        ASSERT_TRUE(remove_first(l, [i](auto c) {                              \
          /**/                                                                 \
          return c == i;                                                       \
        }));                                                                   \
      }                                                                        \
    }                                                                          \
  } while (0)

TEST(LinkedListTest, test_speed_alloc) {
  sp::LinkedList<std::uint64_t> l;
  LinkedList_Test_speed(l);
}

TEST(LinkedListTest, test_speed_stack_alloc) {
  sp::LinkedList<std::uint64_t, sp::StackPooledAllocator> l;
  LinkedList_Test_speed(l);
}

TEST(LinkedListTest, test_remove_if) {
  sp::LinkedList<std::uint64_t> l;
  {
    ASSERT_EQ(std::size_t(0), remove_if(l, [](auto) {
                /**/
                return true;
              }));
  }
  {
    insert(l, 1);
    ASSERT_EQ(std::size_t(0), remove_if(l, [](auto) {
                /**/
                return false;
              }));
    ASSERT_EQ(false, is_empty(l));
    auto c = get(l, 0);
    ASSERT_TRUE(c);
    ASSERT_EQ(*c, 1);
  }
  {
    ASSERT_EQ(false, is_empty(l));
    ASSERT_EQ(std::size_t(1), remove_if(l, [](auto c) {
                /**/
                return c == 1;
              }));
    ASSERT_EQ(true, is_empty(l));
  }
  {
    prng::xorshift32 r(1);
    constexpr std::size_t cap = 1024;
    sp::StaticArray<std::uint64_t, cap> in;

    for (std::uint64_t i = 0; i < cap; ++i) {
      ASSERT_TRUE(insert(in, i));
      insert(l, i);
    }
    ASSERT_EQ(cap, length(in));
    shuffle(r, in);

    for (std::size_t i = 0; i < cap; ++i) {
      ASSERT_EQ(1, remove_if(l, [&in, i](auto c) {
                  /**/
                  return c == in[i];
                }));
    }
    ASSERT_TRUE(is_empty(l));
  }
}

TEST(LinkedListTest, test_reverse) {
  sp::LinkedList<std::size_t> l;
  ASSERT_EQ(l.root, nullptr);
  sp::rec::reverse(l);
  ASSERT_EQ(l.root, nullptr);
  for (std::size_t i = 0; i < 10; ++i) {
    ASSERT_TRUE(insert(l, i));

    {
      sp::rec::reverse(l);
      ASSERT_EQ(l.root->value, i);
      ASSERT_EQ(l.last->value, 0);
      std::size_t cmp = i;
      for (std::size_t a = 0; a <= i; ++a) {
        auto current = get(l, a);
        ASSERT_TRUE(current);
        // printf("%zu,", *current);
        ASSERT_EQ(cmp--, *current);
      }
      // printf("\n");
    }
    {
      sp::rec::reverse(l);
      ASSERT_EQ(l.root->value, 0);
      ASSERT_EQ(l.last->value, i);
      for (std::size_t a = 0; a <= i; ++a) {
        auto current = get(l, a);
        ASSERT_TRUE(current);
        // printf("%zu,", *current);
        ASSERT_EQ(a, *current);
      }
      // printf("\n");
    }
    // printf("---\n");
  }
}
