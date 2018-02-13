#include "gtest/gtest.h"
#include <collection/Array.h>
#include <forward_list>
#include <list/LinkedList.h>
#include <memory/StackPooledAllocator.h>
#include <prng/util.h>
#include <prng/xorshift.h>

struct LinkedListTestNode {
  void *ptr = nullptr;
};

TEST(LinkedListTest, test_ins) {
  sp::LinkedList<int> l;

  {
    ASSERT_FALSE(get(l, 0));
    ASSERT_FALSE(find_first(l, [](auto) {
      assert(false);
      return false;
    }));
    {
      int *r = push_back(l, 1);
      ASSERT_TRUE(r);
      ASSERT_EQ(*r, 1);
    }
    ASSERT_FALSE(get(l, 1));
    {
      int *res = find_first(l, [](auto in) { //
        return in == 1;
      });
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, 1);
    }
    {
      int *r = get(l, 0);
      ASSERT_TRUE(r);
      ASSERT_EQ(*r, 1);
      for_each(l, [](auto in) { //
        assert(in == 1);
      });
      ASSERT_TRUE(for_all(l, [](auto in) { //
        return in == 1;
      }));
    }
    ASSERT_FALSE(get(l, 1));
    remove_first(l, [](auto in) { //
      return in == 1;
    });
    ASSERT_FALSE(get(l, 0));
    ASSERT_TRUE(for_all(l, [](auto) { //
      return false;
    }));
    clear(l);
    ASSERT_FALSE(get(l, 0));
    ASSERT_TRUE(for_all(l, [](auto) { //
      return false;
    }));
  }
  const int max = 100;
  for (int i = 0; i < max; ++i) {
    for (int a = 0; a < i; ++a) {
      int *res = get(l, std::size_t(a));
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, a);
      {
        int *res = find_first(l, [&a](auto in) { //
          return in == a;
        });
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, a);
      }
    }

    int *res = insert(l, i);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);

    for (int a = i + 1; a < max; ++a) {
      int *res = get(l, std::size_t(a));
      ASSERT_FALSE(res);
      {
        ASSERT_FALSE(find_first(l, [&a](auto in) { //
          return in == a;
        }));
      }
    }
  }

  {
    int i = 0;
    for_each(l, [&i](auto in) { //
      assert(in == i++);
    });
    ASSERT_EQ(i, max);
  }

  {
    int i = 0;
    ASSERT_TRUE(for_all(l, [&](auto in) { //
      return in == i++;
    }));
  }

  prng::Xorshift32 r(1);
  sp::StaticArray<int, max> in;
  ASSERT_EQ(in.capacity, std::size_t(max));
  for (int i = 0; i < max; ++i) {
    ASSERT_TRUE(insert(in, i));
  }
  ASSERT_EQ(in.length, std::size_t(max));
  shuffle(r, in);

  for (std::size_t i = 0; i < in.length; ++i) {
    for (std::size_t a = 0; a < i; ++a) {
      ASSERT_FALSE(find_first(l, [&in, a](auto c) { //
        return c == *get(in, a);
      }));
    }

    ASSERT_TRUE(remove_first(l, [&in, i](auto c) { //
      return c == *get(in, i);
    }));

    for (int a = i + 1; a < max; ++a) {
      int *res = find_first(l, [&in, a](auto c) { //
        return c == *get(in, a);
      });
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, *get(in, a));
    }
  }
}

TEST(LinkedListTest, test) {

  sp::LinkedList<LinkedListTestNode /*,sp::StackPooledAllocator*/> l;

  //   std::forward_list<int,sp::StackPooledAllocator<int>> f;
  //   for(int i=0;i<4;++i){
  //     printf("# push_back(%d)\n",i);
  //     f.push_front(i);
  //     printf("\n");
  //   }
  //   printf("-----------\n\n\n");
}
