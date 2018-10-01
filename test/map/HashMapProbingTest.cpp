#include <gtest/gtest.h>
#include <map/HashMapProbing.h>
#include <map/HashMapTree.h>
#include <test/gcstruct.h>

template <typename MAP>
static void
test_simple(MAP &map) {
  using TT = typename MAP::value_type;

  ASSERT_FALSE(lookup(map, 1));
  ASSERT_FALSE(lookup(map, 2));

  {
    TT *const i = insert(map, 1, 2);
    ASSERT_TRUE(i);

    ASSERT_FALSE(lookup(map, 2));

    TT *const l = lookup(map, 1);
    ASSERT_TRUE(l);

    ASSERT_EQ(*i, 2);

    ASSERT_EQ(*l, 2);
    ASSERT_EQ(l, i);
  }
  ASSERT_TRUE(lookup(map, 1));
  ASSERT_FALSE(lookup(map, 2));

  {
    TT *i = insert(map, 2, 3);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 3);

    TT *l = lookup(map, 2);
    ASSERT_TRUE(l);
    ASSERT_EQ(*l, 3);
    ASSERT_EQ(l, i);
  }
  ASSERT_TRUE(lookup(map, 1));
  ASSERT_TRUE(lookup(map, 2));

  {
    TT *i = insert(map, 1, 10);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 10);

    TT *l = lookup(map, 1);
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

TEST(HashMapProbingTest, test_probing_dtor) {
  ASSERT_EQ(0, sp::GcStruct::active);
  {
    sp::HashMapProbing<int, sp::GcStruct> map;
    test_simple(map);
  }
  ASSERT_EQ(0, sp::GcStruct::active);
}

#if 0
TEST(HashMapProbingTest, test_probing_dtor2) {
  ASSERT_EQ(0, sp::GcStruct::active);
  {
    sp::HashMapProbing<sp::GcStruct, int> map;
    test_simple(map);
  }
  ASSERT_EQ(0, sp::GcStruct::active);
}

TEST(HashMapProbingTest, test_probing_dtor3) {
  ASSERT_EQ(0, sp::GcStruct::active);
  {
    sp::HashMapProbing<sp::GcStruct, sp::GcStruct> map;
    test_simple(map);
  }
  ASSERT_EQ(0, sp::GcStruct::active);
}
#endif

TEST(HashMapProbingTest, test_tree) {
  sp::HashMapTree<int, int> map;
  test_simple(map);
}
