#include <gtest/gtest.h>
#include <map/HashMapProbing.h>
#include <map/HashMapTree.h>
#include <test/gcstruct.h>

template <typename MAP>
static void
test_simple(MAP &map) {
  using TK = typename MAP::key_type;
  using TV = typename MAP::value_type;

  const std::size_t c_key_one = 1;
  std::size_t key_one = 1;
  const TK c_tt_one(1);
  TK tt_one(1);
  {
    ASSERT_FALSE(lookup(map, 1));
    ASSERT_FALSE(lookup(map, c_key_one));
    ASSERT_FALSE(lookup(map, key_one));
    ASSERT_EQ(key_one, 1);
    ASSERT_FALSE(lookup(map, c_tt_one));
    ASSERT_FALSE(lookup(map, tt_one));
  }
  { //
    ASSERT_FALSE(lookup(map, 2));
  }

  {
    {
      TV *const i = insert(map, 1, 2);
      ASSERT_TRUE(i);

      ASSERT_FALSE(lookup(map, 2));

      TV *const l = lookup(map, 1);
      ASSERT_TRUE(l);

      ASSERT_EQ(*i, 2);

      ASSERT_EQ(*l, 2);
      ASSERT_EQ(l, i);
    }

    {
      TV *const l = lookup(map, c_key_one);
      ASSERT_TRUE(l);
      ASSERT_EQ(*l, 2);
    }
    {
      TV *const l = lookup(map, key_one);
      ASSERT_TRUE(l);
      ASSERT_EQ(*l, 2);
      ASSERT_EQ(key_one, 1);
    }
    {
      TV *const l = lookup(map, c_tt_one);
      ASSERT_TRUE(l);
      ASSERT_EQ(*l, 2);
    }
    {
      TV *const l = lookup(map, tt_one);
      ASSERT_TRUE(l);
      ASSERT_EQ(*l, 2);
    }
  }
  ASSERT_TRUE(lookup(map, 1));
  ASSERT_FALSE(lookup(map, 2));

  {
    TV *i = insert(map, 2, 3);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 3);

    TV *l = lookup(map, 2);
    ASSERT_TRUE(l);
    ASSERT_EQ(*l, 3);
    ASSERT_EQ(l, i);

    {
      TV *const l = lookup(map, c_key_one);
      ASSERT_TRUE(l);
      ASSERT_EQ(*l, 2);
    }
    {
      TV *const l = lookup(map, key_one);
      ASSERT_TRUE(l);
      ASSERT_EQ(*l, 2);
      ASSERT_EQ(key_one, 1);
    }
    {
      TV *const l = lookup(map, c_tt_one);
      ASSERT_TRUE(l);
      ASSERT_EQ(*l, 2);
    }
    {
      TV *const l = lookup(map, tt_one);
      ASSERT_TRUE(l);
      ASSERT_EQ(*l, 2);
    }
  }
  ASSERT_TRUE(lookup(map, 1));
  ASSERT_TRUE(lookup(map, 2));

  {
    TV *i = insert(map, 1, 10);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 10);

    TV *l = lookup(map, 1);
    ASSERT_TRUE(l);
    ASSERT_EQ(*l, 10);
    ASSERT_EQ(l, i);

    {
      TV *const l = lookup(map, c_key_one);
      ASSERT_TRUE(l);
      ASSERT_EQ(*l, 10);
    }
    {
      TV *const l = lookup(map, key_one);
      ASSERT_TRUE(l);
      ASSERT_EQ(*l, 10);
      ASSERT_EQ(key_one, 1);
    }
    {
      TV *const l = lookup(map, c_tt_one);
      ASSERT_TRUE(l);
      ASSERT_EQ(*l, 10);
    }
    {
      TV *const l = lookup(map, tt_one);
      ASSERT_TRUE(l);
      ASSERT_EQ(*l, 10);
    }
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

TEST(HashMapProbingTest, test_tree) {
  sp::HashMapTree<int, int> map;
  test_simple(map);
}

TEST(HashMapProbingTest, test_tree_dtor) {
  ASSERT_EQ(0, sp::GcStruct::active);
  {
    sp::HashMapTree<int, sp::GcStruct> map;
    test_simple(map);
  }
  ASSERT_EQ(0, sp::GcStruct::active);
}

TEST(HashMapProbingTest, test_tree_dtor2) {
  ASSERT_EQ(0, sp::GcStruct::active);
  {
    sp::HashMapTree<sp::GcStruct, int> map;
    test_simple(map);
  }
  ASSERT_EQ(0, sp::GcStruct::active);
}

TEST(HashMapProbingTest, test_tree_dtor3) {
  ASSERT_EQ(0, sp::GcStruct::active);
  {
    sp::HashMapTree<sp::GcStruct, sp::GcStruct> map;
    test_simple(map);
  }
  ASSERT_EQ(0, sp::GcStruct::active);
}

template <typename MAP>
static void
test_gc_value(MAP &map) {
  sp::GcStruct::ctor = 0;
  ASSERT_EQ(0, sp::GcStruct::ctor);

  {
    sp::GcStruct *const i = insert(map, 0, 1);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 1);
    ASSERT_EQ(1, sp::GcStruct::ctor);
  }
  sp::GcStruct::ctor = 0;

  {
    const int c_value = 3;
    sp::GcStruct *const i = insert(map, 2, c_value);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 3);
    ASSERT_EQ(1, sp::GcStruct::ctor);
  }
  sp::GcStruct::ctor = 0;

  {
    int value = 4;
    sp::GcStruct *const i = insert(map, 3, value);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 4);
    ASSERT_EQ(1, sp::GcStruct::ctor);
    ASSERT_EQ(4, value);
  }
  sp::GcStruct::ctor = 0;

#if 0
  {
    const sp::GcStruct c_value(6);
    sp::GcStruct *const i = insert(map, 5, c_value);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 6);
    ASSERT_EQ(1, sp::GcStruct::ctor);
  }
  sp::GcStruct::ctor = 0;

  {
    sp::GcStruct value(8);
    sp::GcStruct *const i = insert(map, 7, value);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 8);
    ASSERT_EQ(1, sp::GcStruct::ctor);
    ASSERT_EQ(value, 8);
  }
  sp::GcStruct::ctor = 0;
#endif

#if 0
  {
    sp::GcStruct *const i = insert(map, 9, sp::GcStruct(10));
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 10);
    ASSERT_EQ(1, sp::GcStruct::ctor);
  }
  sp::GcStruct::ctor = 0;

  {
    sp::GcStruct value(12);
    sp::GcStruct *const i = insert(map, 11, std::move(value));
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 12);
    ASSERT_EQ(1, sp::GcStruct::ctor);
  }
  sp::GcStruct::ctor = 0;
#endif
}

TEST(HashMapProbingTest, test_probing_gc_value) {
  sp::HashMapProbing<int, sp::GcStruct> map;
  test_gc_value(map);
}

TEST(HashMapProbingTest, test_tree_gc_value) {
  sp::HashMapTree<int, sp::GcStruct> map;
  test_gc_value(map);
}

template <typename MAP>
static void
test_gc_key(MAP &map) {
  sp::GcStruct::ctor = 0;
  ASSERT_EQ(0, sp::GcStruct::ctor);

  {
    int *const i = insert(map, 0, 1);
    ASSERT_TRUE(i);
    ASSERT_EQ(1, sp::GcStruct::ctor);
  }
  sp::GcStruct::ctor = 0;

  {
    const int c_key = 3;
    int *const i = insert(map, c_key, 2);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 2);
    ASSERT_EQ(1, sp::GcStruct::ctor);
  }
  sp::GcStruct::ctor = 0;

  {
    int key = 4;
    int *const i = insert(map, key, 3);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 3);
    ASSERT_EQ(1, sp::GcStruct::ctor);
    ASSERT_EQ(4, key);
  }
  sp::GcStruct::ctor = 0;

#if 0
  {
    const sp::GcStruct c_key(6);
    int *const i = insert(map, c_key, 5);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 5);
    ASSERT_EQ(1, sp::GcStruct::ctor);
  }
  sp::GcStruct::ctor = 0;

  {
    sp::GcStruct key(8);
    int *const i = insert(map, key, 7);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 7);
    ASSERT_EQ(1, sp::GcStruct::ctor);
    ASSERT_EQ(key, 8);
  }
  sp::GcStruct::ctor = 0;
#endif

#if 0
  {
    int *const i = insert(map, sp::GcStruct(10), 9);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 9);
    ASSERT_EQ(1, sp::GcStruct::ctor);
  }
  sp::GcStruct::ctor = 0;

  {
    sp::GcStruct key(12);
    int *const i = insert(map, std::move(key), 11);
    ASSERT_TRUE(i);
    ASSERT_EQ(*i, 12);
    ASSERT_EQ(1, sp::GcStruct::ctor);
  }
  sp::GcStruct::ctor = 0;

  {
    sp::GcStruct key(12);
    int *const i = insert(map, 11, std::move(key));
    ASSERT_TRUE(i);
    ASSERT_EQ(1, sp::GcStruct::ctor);
  }
  sp::GcStruct::ctor = 0;
#endif
}

TEST(HashMapProbingTest, test_probing_gc_key) {
  sp::HashMapProbing<sp::GcStruct, int> map;
  test_gc_key(map);
}

TEST(HashMapProbingTest, test_tree_gc_key) {
  sp::HashMapTree<sp::GcStruct, int> map;
  test_gc_key(map);
}
