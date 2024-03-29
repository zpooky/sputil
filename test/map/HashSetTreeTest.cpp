#include <gtest/gtest.h>
#include <map/HashSetProbing.h>
#include <map/HashSetTree.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <test/gcstruct.h>
#include <unordered_set>
#include <util/Bitset.h>
#include <util/assert.h>

// #include <hash/fnv.h>
// #include <list/SkipList.h>

template <typename SET>
static void
test_insert_dtor(SET &set) {
  using TType = typename SET::value_type;

  for (std::size_t i = 0; i < 1024; ++i) {
    for (std::size_t a = 0; a < i; ++a) {
      {
        TType *res = insert(set, TType(a, 0));
        if (res) {
          printf("i[%zu] a[%zu] = res[%zu]\n", i, a, std::size_t(*res));
          // dump(set.tree);
        }
        ASSERT_FALSE(res);
      }
      {
        const TType *res = lookup(set, TType(a, 0));
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, a);
      }
      {
        const TType *res = lookup(set, a);
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, a);
      }
    }

    using namespace sp::rec;
    ASSERT_EQ(i, length(set));
    {
      ASSERT_FALSE(lookup(set, i));
      ASSERT_FALSE(lookup(set, TType(i, 0)));

      TType *res = insert(set, TType(i, 0));
      ASSERT_TRUE(res);
      ASSERT_EQ(res->data, i);
      ASSERT_TRUE(verify(set));
    }
  }
}

TEST(HashSetTreeTest, test_Tree_insert_dtor) {
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
  {
    sp::HashSetTree<sp::GcStruct> set;
    test_insert_dtor(set);
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}

TEST(HashSetTreeTest, test_Probing_insert_dtor) {
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
  {
    sp::HashSetProbing<sp::GcStruct> set;
    test_insert_dtor(set);
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}

template <typename SET>
static void
test_insert_remove(SET &set) {
  using TType = typename SET::value_type;
  using namespace sp::rec;

  for (std::size_t i = 0; i < 1024; ++i) {
    for (std::size_t a = 0; a < i; ++a) {
      {
        TType *res = insert(set, TType(a, 0));
        if (res) {
          printf("i[%zu] a[%zu] = res[%zu]\n", i, a, std::size_t(*res));
          // dump(set.tree);
        }
        ASSERT_FALSE(res);
      }
      {
        const TType *res = lookup(set, TType(a, 0));
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, a);
      }
      {
        const TType *res = lookup(set, a);
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, a);
      }
    }

    ASSERT_EQ(i, length(set));
    {
      ASSERT_FALSE(lookup(set, i));
      ASSERT_FALSE(lookup(set, TType(i, 0)));

      TType *res = insert(set, TType(i, 0));
      ASSERT_TRUE(res);
      ASSERT_EQ(res->data, i);
      ASSERT_TRUE(verify(set));
    }
  }
  for (std::size_t i = 0; i < 1024; ++i) {
    TType *res = lookup(set, i);
    ASSERT_TRUE(res);
    ASSERT_EQ(res->data, i);
    ASSERT_TRUE(remove(set, i));
    ASSERT_FALSE(lookup(set, i));
  }

  ASSERT_EQ(std::size_t(0), length(set));
}

TEST(HashSetTreeTest, test_Tree_insert_remove) {
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
  {
    sp::HashSetTree<sp::GcStruct> set;
    test_insert_remove(set);
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}

// #if 0
TEST(HashSetTreeTest, test_Probing_insert_remove) {
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
  {
    sp::HashSetProbing<sp::GcStruct> set;
    test_insert_remove(set);
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}
// #endif

template <typename SET>
static void
test_upsert_dtor(SET &set) {
  using TType = typename SET::value_type;

  for (std::size_t i = 0; i < 1024; ++i) {
    for (std::size_t a = 0; a < i; ++a) {
      {
        TType *res = insert(set, TType(a, 0));
        ASSERT_FALSE(res);
      }
      {
        TType *res = upsert(set, TType(a, 0));
        ASSERT_TRUE(res);
      }
      {
        const TType *res = lookup(set, TType(a, 0));
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, a);
      }
      {
        const TType *res = lookup(set, a);
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, a);
      }
    }

    using namespace sp::rec;
    ASSERT_EQ(i, length(set));
    {
      ASSERT_FALSE(lookup(set, i));
      ASSERT_FALSE(lookup(set, TType(i, 0)));

      TType *res = upsert(set, TType(i, 0));
      ASSERT_TRUE(res);
      ASSERT_EQ(res->data, i);
      ASSERT_TRUE(verify(set));
    }
  }
}

TEST(HashSetTreeTest, test_Tree_upsert_dtor) {
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
  {
    sp::HashSetTree<sp::GcStruct> set;
    test_upsert_dtor(set);
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}

TEST(HashSetTreeTest, test_Probing_upsert_dtor) {
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
  {
    sp::HashSetProbing<sp::GcStruct> set;
    test_upsert_dtor(set);
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}

template <typename SET>
static void
test_lookup_insert_dtor(SET &set) {
  using TType = typename SET::value_type;

  for (std::size_t i = 0; i < 1024; ++i) {
    for (std::size_t a = 0; a < i; ++a) {
      {
        TType *res = insert(set, TType(a, 0));
        ASSERT_FALSE(res);
      }
      {
        TType *res = upsert(set, TType(a, 0));
        ASSERT_TRUE(res);
      }
      {
        TType *res = lookup_insert(set, TType(a, 0));
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, a);
      }
      {
        const TType *res = lookup(set, TType(a, 0));
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, a);
      }
      {
        const TType *res = lookup(set, a);
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, a);
      }
    }

    using namespace sp::rec;

    ASSERT_EQ(i, length(set));
    {
      ASSERT_FALSE(lookup(set, i));
      ASSERT_FALSE(lookup(set, TType(i, 0)));

      TType *res = lookup_insert(set, TType(i, 0));
      ASSERT_TRUE(res);
      ASSERT_EQ(res->data, i);
      ASSERT_TRUE(verify(set));
    }
  }
}

TEST(HashSetTreeTest, test_Tree_lookup_insert_dtor) {
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
  {
    sp::HashSetTree<sp::GcStruct> set;
    test_lookup_insert_dtor(set);
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}

TEST(HashSetTreeTest, test_Probing_lookup_insert_dtor) {
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
  {
    sp::HashSetProbing<sp::GcStruct> set;
    test_lookup_insert_dtor(set);
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}

template <typename SET>
static void
test_lookup_compute_dtor(SET &set) {
  using TType = typename SET::value_type;

  auto not_comp = [](TType &, const TType &) {
    /**/
    assertx(false);
  };

  for (std::size_t i = 0; i < 1024; ++i) {

    auto comp = [&i](TType &current, const TType &xarx) {
      assertxs(xarx.data == i, xarx.data, i);
      new (&current) TType(xarx.data, 0);
    };

    for (std::size_t a = 0; a < i; ++a) {
      {
        // printf("insert()\n");
        TType *res = insert(set, TType(a, 0));
        ASSERT_FALSE(res);
      }
      {
        // printf("lookup_compute()\n");
        TType *res = lookup_compute(set, TType(a, 0), not_comp);
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, a);
      }
      {
        // printf("upsert()\n");
        TType *res = upsert(set, TType(a, 0));
        ASSERT_TRUE(res);
      }
      {
        // printf("lookup_insert()\n");
        TType *res = lookup_insert(set, TType(a, 0));
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, a);
      }
      {
        // printf("lookup(TType())\n");
        const TType *res = lookup(set, TType(a, 0));
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, a);
      }
      {
        // printf("lookup()\n");
        const TType *res = lookup(set, a);
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, a);
      }
    } // for
    // printf("---\n");

    using namespace sp::rec;
    ASSERT_EQ(i, length(set));
    {
      ASSERT_FALSE(lookup(set, i));
      ASSERT_FALSE(lookup(set, TType(i, 0)));

      // printf("__lookup_compute(%zu){", i);
      TType *res = lookup_compute(set, TType(i, 0), comp);
      // printf("}\n");
      ASSERT_TRUE(res);
      ASSERT_EQ(res->data, i);
      ASSERT_TRUE(verify(set));
    }
  } // for
}

TEST(HashSetTreeTest, test_Tree_lookup_compute_dtor) {
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
  {
    sp::HashSetTree<sp::GcStruct> set;
    test_lookup_compute_dtor(set);
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}

TEST(HashSetTreeTest, test_Probing_lookup_compute_dtor) {
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
  {
    sp::HashSetProbing<sp::GcStruct> set;
    test_lookup_compute_dtor(set);
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}

TEST(HashSetTreeTest, test_vanilla) {
  using TType = sp::GcStruct;
  {
    std::unordered_set<TType, sp::Hasher<TType>> set;
    for (std::size_t i = 0; i < 1024; ++i) {
      for (std::size_t a = 0; a < i; ++a) {
        auto res = set.insert(TType(a, 0));
        ASSERT_FALSE(res.second);
        {
          TType needle(a, 0);
          auto res = set.find(needle);
          ASSERT_TRUE(res != set.end());
          ASSERT_EQ(res->data, a);
        }
      }

      {
        auto res = set.insert(TType(i, 0));
        ASSERT_TRUE(res.second);
        ASSERT_EQ(res.first->data, i);
      }
    }
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}

template <typename SET>
static void
test_rand(prng::xorshift32 &r) {
  constexpr std::uint32_t range = 1024 * 9;
  constexpr std::size_t bits(sizeof(std::uint64_t) * 8);
  ASSERT_TRUE(range % bits == 0);
  const std::size_t length = range / bits;
  uint64_t *const raw = new uint64_t[length]{0};

  do {
    memset(raw, 0, length);
    SET set;
    sp::Bitset bset(raw, length);

    for (std::size_t i = 0; i < range; ++i) {

      for_each(bset, [&set](std::size_t idx, bool v) {
        const std::uint32_t in(idx);
        auto res = lookup(set, in);
        if (v) {
          ASSERT_TRUE(res);
          ASSERT_EQ(*res, in);
        } else {
          ASSERT_FALSE(res);
        }
      });

      const std::uint32_t current = uniform_dist(r, 0, range);
      {
        const bool v = test(bset, current);
        auto res = insert(set, std::size_t(current));
        if (v) {
          ASSERT_FALSE(res);
        } else {
          ASSERT_TRUE(res);
          ASSERT_EQ(*res, current);
          bool priv = sp::set(bset, std::size_t(current), true);
          ASSERT_EQ(false, priv);
        }
      }

    } // for
    ASSERT_TRUE(sp::rec::verify(set));
    // dump(set.tree);

  } while (false);

  delete[] raw;
}

TEST(HashSetTreeTest, test_Tree_rand) {
  prng::xorshift32 r(1);
  test_rand<sp::HashSetTree<std::uint32_t>>(r);
}

TEST(HashSetTreeTest, test_Probing_rand) {
  prng::xorshift32 r(1);
  test_rand<sp::HashSetProbing<std::uint32_t>>(r);
}

template <typename SET>
static void
test_dtor(SET &set) {
  for (std::size_t i = 0; i < 1024; ++i) {
    for (std::size_t a = 0; a < i; ++a) {
      {
        auto *res = insert(set, a);
        ASSERT_FALSE(res);
      }
      {
        auto *res = lookup(set, sp::GcStruct(a));
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, a);
      }
      {
        auto *res = lookup(set, a);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, a);
      }
    }
    {
      auto *res = insert(set, i);

      ASSERT_TRUE(res);
      ASSERT_EQ(*res, i);
    }
  }
}

TEST(HashSetTreeTest, test_Tree_dtor) {
  {
    sp::HashSetTree<sp::GcStruct> set;
    test_dtor(set);
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}

TEST(HashSetTreeTest, test_Probing_dtor) {
  {
    sp::HashSetProbing<sp::GcStruct> set;
    test_dtor(set);
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}

template <typename SET>
static void
test_afew(prng::xorshift32 &r, SET &set_insert, SET &set_upsert,
          SET &set_look_insert, SET &set_compute) {

  std::size_t inserted = 0;
  std::size_t i = 0;
  constexpr std::size_t limit_i = 31000 /*00*/;
  constexpr std::size_t max_dist = 3233123;

  sp::DynamicBitset b(sp::Bitset_number_of_buffer(max_dist));
  for (std::size_t i = 0; i < capacity(b); ++i) {
    ASSERT_FALSE(sp::set(b, i, true));
    ASSERT_TRUE(sp::set(b, i, false));
  }

  ASSERT_FALSE(sp::set(b, max_dist, true));
  ASSERT_TRUE(sp::set(b, max_dist, false));

  while (i++ < limit_i) {
    if (i % 10000 == 0) {
      // printf(".%zu\n", i);
    }
    const std::size_t in = uniform_dist(r, 0, max_dist);
    std::uint32_t *res = insert(set_insert, in);

    bool comp = false;
    auto f = [in, &comp](std::uint32_t &current, const auto &xarx) {
      comp = true;
      assertxs(in == xarx, in, xarx);
      return new (&current) std::uint32_t(xarx);
    };
    ASSERT_TRUE(lookup_compute(set_compute, in, f));

    if (res) {
      ASSERT_EQ(*res, in);
      ++inserted;
      ASSERT_FALSE(test(b, in));
      ASSERT_FALSE(sp::set(b, in, true));
      ASSERT_TRUE(comp);
    } else {
      // already inserted
      ASSERT_TRUE(test(b, in));
      ASSERT_FALSE(comp);
    }
    ASSERT_TRUE(upsert(set_upsert, in));
    ASSERT_TRUE(lookup_insert(set_look_insert, in));
  }

  using namespace sp::rec;
  ASSERT_EQ(inserted, length(set_insert));
  ASSERT_EQ(inserted, length(set_upsert));
  ASSERT_EQ(inserted, length(set_look_insert));
  ASSERT_EQ(inserted, length(set_compute));
  printf("inserted:%zu\n", inserted);

  std::size_t fins = 0;
  for_each(b, [&](std::size_t idx, bool present) {
    ASSERT_EQ(test(b, idx), present);

    std::uint32_t *l_ins = lookup(set_insert, idx);
    std::uint32_t *l_ups = lookup(set_upsert, idx);
    std::uint32_t *l_look_ins = lookup(set_look_insert, idx);
    std::uint32_t *l_comp = lookup(set_look_insert, idx);
    if (present) {
      ++fins;
      ASSERT_TRUE(l_ins);
      ASSERT_EQ(idx, *l_ins);

      ASSERT_TRUE(l_ups);
      ASSERT_EQ(idx, *l_ups);

      ASSERT_TRUE(l_look_ins);
      ASSERT_EQ(idx, *l_look_ins);

      ASSERT_TRUE(l_comp);
      ASSERT_EQ(idx, *l_comp);
    } else {
      ASSERT_FALSE(l_ins);
      ASSERT_FALSE(l_ups);
      ASSERT_FALSE(l_look_ins);
      ASSERT_FALSE(l_comp);
    }
  });

  ASSERT_EQ(inserted, fins);
  printf("verify\n");
  verify(set_insert);
  verify(set_upsert);
  verify(set_look_insert);
  verify(set_compute);
  printf("done\n");
}

TEST(HashSetTreeTest, test_Tree_afew) {
  prng::xorshift32 r(20000);
  // for (; seed < 10000000; ++seed) //
  {
    sp::HashSetTree<std::uint32_t> set_insert;
    sp::HashSetTree<std::uint32_t> set_upsert;
    sp::HashSetTree<std::uint32_t> set_look_insert;
    sp::HashSetTree<std::uint32_t> set_compute;
    test_afew(r, set_insert, set_upsert, set_look_insert, set_compute);
  }
}

TEST(HashSetTreeTest, test_Probing_afew) {
  prng::xorshift32 r(20000);
  // for (; seed < 10000000; ++seed) //
  {
    sp::HashSetProbing<std::uint32_t> set_insert;
    sp::HashSetProbing<std::uint32_t> set_upsert;
    sp::HashSetProbing<std::uint32_t> set_look_insert;
    sp::HashSetProbing<std::uint32_t> set_compute;
    test_afew(r, set_insert, set_upsert, set_look_insert, set_compute);
  }
}

TEST(HashSetTreeTest, test_lookup_insert) {
  sp::HashSetTree<std::uint32_t> set;
  ASSERT_EQ(std::size_t(0), sp::rec::length(set));
  std::uint32_t *first = lookup_insert(set, 1);
  ASSERT_EQ(std::size_t(1), sp::rec::length(set));
  ASSERT_EQ(*first, 1);
  std::uint32_t *second = lookup_insert(set, 1);
  ASSERT_EQ(*first, 1);
  ASSERT_EQ(*first, *second);
  ASSERT_EQ(first, second);
  ASSERT_EQ(std::size_t(1), sp::rec::length(set));
}

TEST(HashSetTreeTest, test_lookup_compute) {
  sp::HashSetTree<std::uint32_t> set;
  ASSERT_EQ(0, sp::rec::length(set));
  bool first_cb = false;

  auto f = [&first_cb](std::uint32_t &current, const std::uint32_t &value) {
    first_cb = true;
    return new (&current) std::uint32_t(value);
  };
  std::uint32_t *first = lookup_compute(set, 1, f);

  ASSERT_TRUE(first_cb);
  ASSERT_EQ(1, sp::rec::length(set));
  ASSERT_EQ(*first, 1);

  bool second_cb = false;
  auto f2 = [&second_cb](std::uint32_t &, const std::uint32_t &) {
    second_cb = true;
    return nullptr;
  };
  std::uint32_t *second = lookup_compute(set, 1, f2);
  ASSERT_FALSE(second_cb);
  ASSERT_EQ(*first, 1);
  ASSERT_EQ(*first, *second);
  ASSERT_EQ(first, second);
  ASSERT_EQ(1, sp::rec::length(set));
}

#if 0

TEST(HashSetTreeTest, test_lookup_compute2) {
  // sp::LinkedList<int> nodes;
  std::size_t insxx = 0;

  auto factory = [&insxx](auto &state, int id) {
    insxx++;
    // printf("  2.push_back(%d)\n", id);
    // int *res = push_back(nodes, id);
    // assertx(res);

    new (&state) int(id);
  };

  sp::HashSetTree<int> set;

  int *zero = lookup_compute(set, 0, factory);
  ASSERT_TRUE(zero);
  ASSERT_EQ(*zero, 0);
  ASSERT_EQ(1, insxx);
  ASSERT_EQ(insxx, sp::rec::length(set));

  int *one = lookup_compute(set, 1, factory);
  ASSERT_TRUE(one);
  ASSERT_EQ(*one, 1);
  ASSERT_EQ(2, insxx);
  ASSERT_EQ(insxx, sp::rec::length(set));

  int *three = lookup_compute(set, 3, factory);
  ASSERT_TRUE(three);
  ASSERT_EQ(*three, 3);
  ASSERT_EQ(3, insxx);
  ASSERT_EQ(insxx, sp::rec::length(set));

  int *three_x = lookup_compute(set, 3, factory);
  ASSERT_TRUE(three_x);
  ASSERT_EQ(*three_x, 3);
  ASSERT_EQ(three, three_x);
  ASSERT_EQ(3, insxx);
  ASSERT_EQ(insxx, sp::rec::length(set));

  int *one_x = lookup_compute(set, 1, factory);
  ASSERT_TRUE(one_x);
  ASSERT_EQ(*one_x, 1);
  ASSERT_EQ(one_x, one);
  ASSERT_EQ(3, insxx);
  ASSERT_EQ(insxx, sp::rec::length(set));

  printf("--\n");
  int *minus_one = lookup_compute(set, -1, factory);
  printf("--\n");
  ASSERT_TRUE(minus_one);
  ASSERT_EQ(*minus_one, -1);
  ASSERT_EQ(4, insxx);
  ASSERT_EQ(insxx, sp::rec::length(set));

  ASSERT_EQ(*zero, 0);
  ASSERT_EQ(*one, 1);
  ASSERT_EQ(*three, 3);
  ASSERT_EQ(three_x, three);
  ASSERT_EQ(one_x, one);
  ASSERT_EQ(*minus_one, -1);

  {
    int *l_zero = lookup(set, 0);
    ASSERT_EQ(l_zero, zero);
    int *l_one = lookup(set, 1);
    ASSERT_EQ(l_one, one);
    int *l_three = lookup(set, 3);
    ASSERT_EQ(l_three, three);
    int *l_minus_one = lookup(set, -1);
    ASSERT_EQ(l_minus_one, minus_one);
  }
}

#endif

template <typename SET>
static void
test_lookup_compute3(SET &set) {
  sp::GcStruct one_storage{1};

  std::size_t insxx = 0;
  auto factory2 = [&insxx, &one_storage](sp::GcStruct *&state, int id) {
    printf("--\n");
    assertx(id == 1);
    insxx++;
    new (&state) sp::GcStruct *(&one_storage);

    sp::Hasher<sp::GcStruct> hash;
    std::size_t stor_hash = hash(one_storage);
    std::size_t id_hash = hash(id);

    std::size_t st_hash = hash(*state);

    assertxs(stor_hash == id_hash, stor_hash, id_hash);
    assertxs(st_hash == id_hash, stor_hash, st_hash, id_hash);
  };

  auto factory = [factory2](sp::GcStruct *&state, sp::GcStruct *id) {
    assertx(id);

    factory2(state, id->data);
  };

  using namespace sp::rec;
  for (int i = 0; i < 100; ++i) {
    sp::GcStruct one_in{1};
    sp::GcStruct **one = lookup_compute(set, &one_in, factory);
    ASSERT_TRUE(one);
    ASSERT_TRUE(*one);
    ASSERT_EQ((*one)->data, 1);
    ASSERT_EQ(1, insxx);
    ASSERT_EQ(1, length(set));

    {
      sp::GcStruct **one_x = lookup_compute(set, &one_in, factory);
      ASSERT_TRUE(one_x);
      ASSERT_TRUE(*one_x);
      ASSERT_EQ(one, one_x);
      ASSERT_EQ(*one, *one_x);
      ASSERT_EQ((*one_x)->data, 1);
      ASSERT_EQ(1, insxx);
      ASSERT_EQ(1, length(set));
    }

    {
      sp::GcStruct **l_one = lookup(set, 1);
      ASSERT_TRUE(l_one);
      ASSERT_TRUE(*l_one);
      ASSERT_EQ((*l_one)->data, 1);
      ASSERT_EQ(one, l_one);
      ASSERT_EQ(*one, *l_one);
      ASSERT_EQ(1, length(set));
    }

    {
      sp::GcStruct **one_x = lookup_compute(set, 1, factory2);
      ASSERT_TRUE(one_x);
      ASSERT_TRUE(*one_x);
      ASSERT_EQ(one, one_x);
      ASSERT_EQ(*one, *one_x);
      ASSERT_EQ((*one_x)->data, 1);
      ASSERT_EQ(1, insxx);
      ASSERT_EQ(1, length(set));
    }
  }
}

TEST(HashSetTreeTest, test_Tree_lookup_compute3) {
  sp::HashSetTree<sp::GcStruct *> set;
  test_lookup_compute3(set);
}

TEST(HashSetTreeTest, test_Probing_lookup_compute3) {
  sp::HashSetProbing<sp::GcStruct *> set;
  test_lookup_compute3(set);
}

TEST(HashSetTreeTest, hasherxx) {
  {
    sp::Hasher<signed char> h;
    std::size_t hash = h('c');
    printf("signed char: %zu\n", hash);
  }
  {
    sp::Hasher<unsigned char> h;
    std::size_t hash = h('c');
    printf("unsigned char: %zu\n", hash);
  }
}
