#include <gtest/gtest.h>
#include <map/HashMapProbing.h>
#include <map/HashMapTree.h>

template <typename MAP>
static void
test_simple(MAP &map) {
  ASSERT_FALSE(lookup(map, 1));
  ASSERT_FALSE(lookup(map, 2));

  {
    int *i = insert(map, 1, 2);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 2);

    int *l = lookup(map, 1);
    ASSERT_TRUE(l);
    ASSERT_EQ(*l, 2);
    ASSERT_EQ(l, i);
  }
  ASSERT_TRUE(lookup(map, 1));
  ASSERT_FALSE(lookup(map, 2));

  {
    int *i = insert(map, 2, 3);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 3);

    int *l = lookup(map, 2);
    ASSERT_TRUE(l);
    ASSERT_EQ(*l, 3);
    ASSERT_EQ(l, i);
  }
  ASSERT_TRUE(lookup(map, 1));
  ASSERT_TRUE(lookup(map, 2));

  {
    int *i = insert(map, 1, 10);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 10);

    int *l = lookup(map, 1);
    ASSERT_TRUE(l);
    ASSERT_EQ(*l, 10);
    ASSERT_EQ(l, i);
  }
  ASSERT_TRUE(lookup(map, 1));
  ASSERT_TRUE(lookup(map, 2));
}

TEST(HashMapProbingTest, test_probing) {
  sp::HashMapProbing<int, int> map;
  test_simple(map);
}

TEST(HashMapProbingTest, test_tree) {
  sp::HashMapTree<int, int> map;
  test_simple(map);
}
