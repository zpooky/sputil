#include <gtest/gtest.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <tree/avl_rec.h>
#include <tree/btree_extra.h>
#include <tree/btree_rec.h>
#include <util/Bitset.h>

template <typename Tree, typename V>
static typename Tree::value_type *
a_insert(Tree &tree, V v) {
  {
    auto res = find(tree, v);
    assertx(res == nullptr);
  }
  // printf("insert(%d)\n", v);
  auto res = insert(tree, v);
  // btree::impl::btree::dump(tree.root);
  // printf("=============\n");
  assertx(res);
  assertxs(*res == v, *res, v);
  {
    auto res2 = find(tree, v);
    assertx(res2);
    assertxs(*res2 == v, *res2, v);
    assertxs(res2 == res, *res2, *res);
  }
  {
    auto res2 = insert(tree, v);
    assertx(res2);
    assertxs(*res2 == v, *res2, v);
    assertxs(res2 == res, *res2, *res);
  }
  {
    auto res2 = find(tree, v);
    assertx(res2);
    assertxs(*res2 == v, *res2, v);
    assertxs(res2 == res, *res2, *res);
  }

  return res;
}

template <typename Tree, typename V>
static bool
a_remove(Tree &tree, const V &v) {
  {
    auto res = find(tree, v);
    assertx(res);
    assertxs(*res == v, *res, v);
  }
  // printf("==============\n");
  // printf("remove(tree, %d)\n", int(v));
  bool res = remove(tree, v);
  assertx(res);
  // btree::impl::btree::dump(tree.root);
  {
    auto res = find(tree, v);
    assertx(res == nullptr);
  }
  {
    bool res = remove(tree, v);
    assertx(!res);
  }
  return res;
}

template <std::size_t n>
static void
dump(sp::UinStaticArray<int, n> &elements) {
  for (std::size_t i = 0; i < length(elements); ++i) {
    printf("%d, ", elements[i]);
  }
  printf("\n");
}

TEST(btree_recTest, test_median_2_lt) {
  using cmp = sp::less;
  constexpr std::size_t max = 2;
  sp::UinStaticArray<int, max> elements;
  sp::bin_insert<int, max, int, cmp>(elements, 3);
  sp::bin_insert<int, max, int, cmp>(elements, 2);
  // dump(elements);

  int extra = 1;
  const int *res = sp::rec::impl::median<int, max, cmp>(elements, &extra);
  ASSERT_TRUE(res);
  ASSERT_EQ(*res, 2);
}

TEST(btree_recTest, test_median_2_gt) {
  using cmp = sp::greater;
  constexpr std::size_t max = 2;
  sp::UinStaticArray<int, max> elements;
  sp::bin_insert<int, max, int, cmp>(elements, 3);
  sp::bin_insert<int, max, int, cmp>(elements, 2);
  // dump(elements);

  int extra = 1;
  const int *res = sp::rec::impl::median<int, max, cmp>(elements, &extra);
  ASSERT_TRUE(res);
  ASSERT_EQ(*res, 2);
}

TEST(btree_recTest, test_median_3_gt) {
  using cmp = sp::greater;
  constexpr std::size_t max = 3;
  sp::UinStaticArray<int, max> elements;
  sp::bin_insert<int, max, int, cmp>(elements, 0);
  sp::bin_insert<int, max, int, cmp>(elements, 1);
  sp::bin_insert<int, max, int, cmp>(elements, 2);
  // dump(elements);

  int extra = 3;
  const int *res = sp::rec::impl::median<int, max, cmp>(elements, &extra);
  ASSERT_TRUE(res);
  ASSERT_EQ(*res, 1);
  printf("med:%d\n", *res);
}

TEST(btree_recTest, test_fixup_2_gt) {
  using cmp = sp::greater;
  constexpr std::size_t keys = 2;

  sp::rec::BTNode<int, keys, cmp> tree;
  sp::bin_insert<int, keys, int, cmp>(tree.elements, 0);
  sp::bin_insert<int, keys, int, cmp>(tree.elements, 1);
  ASSERT_TRUE(is_full(tree.elements));
  for (std::size_t i = 0; i < capacity(tree.children); ++i) {
    ASSERT_TRUE(insert(tree.children, nullptr));
  }
  // dump(elements);

  int bubble = 2;
  int out = 999;
  int *optr = &out;
  sp::rec::BTNode<int, keys, cmp> *gt = nullptr;
  auto res = sp::rec::impl::fixup(&tree, &bubble, gt, optr);
  {
    int *out_bubble = std::get<0>(res);
    ASSERT_TRUE(out_bubble);

    sp::rec::BTNode<int, keys, cmp> *right = std::get<1>(res);
    ASSERT_TRUE(right);
    ASSERT_EQ(std::size_t(1), length(right->elements));
    ASSERT_EQ(2, right->elements[0]);
    delete right;

    ASSERT_EQ(1, *out_bubble);
  }
  ASSERT_EQ(std::size_t(1), length(tree.elements));
  ASSERT_EQ(0, tree.elements[0]);
}

TEST(btree_recTest, test_fixup_3_gt) {
  using cmp = sp::greater;
  constexpr std::size_t keys = 3;

  sp::rec::BTNode<int, keys, cmp> tree;
  sp::bin_insert<int, keys, int, cmp>(tree.elements, 0);
  sp::bin_insert<int, keys, int, cmp>(tree.elements, 1);
  sp::bin_insert<int, keys, int, cmp>(tree.elements, 2);
  ASSERT_TRUE(is_full(tree.elements));
  for (std::size_t i = 0; i < capacity(tree.children); ++i) {
    ASSERT_TRUE(insert(tree.children, nullptr));
  }
  // dump(elements);

  int bubble = 3;
  int out = 999;
  int *optr = &out;
  sp::rec::BTNode<int, keys, cmp> *gt = nullptr;
  auto res = sp::rec::impl::fixup(&tree, &bubble, gt, optr);
  {
    int *out_bubble = std::get<0>(res);
    ASSERT_TRUE(out_bubble);

    sp::rec::BTNode<int, keys, cmp> *right = std::get<1>(res);
    ASSERT_TRUE(right);
    ASSERT_EQ(std::size_t(2), length(right->elements));
    ASSERT_EQ(2, right->elements[0]);
    ASSERT_EQ(3, right->elements[1]);
    delete right;

    ASSERT_EQ(1, *out_bubble);
  }
  ASSERT_EQ(std::size_t(1), length(tree.elements));
  ASSERT_EQ(0, tree.elements[0]);
}

TEST(btree_recTest, test_fixup_4_gt) {
  using cmp = sp::greater;
  constexpr std::size_t keys = 4;

  sp::rec::BTNode<int, keys, cmp> tree;
  sp::bin_insert<int, keys, int, cmp>(tree.elements, 0);
  sp::bin_insert<int, keys, int, cmp>(tree.elements, 1);
  sp::bin_insert<int, keys, int, cmp>(tree.elements, 2);
  sp::bin_insert<int, keys, int, cmp>(tree.elements, 3);
  ASSERT_TRUE(is_full(tree.elements));
  for (std::size_t i = 0; i < capacity(tree.children); ++i) {
    ASSERT_TRUE(insert(tree.children, nullptr));
  }
  // dump(elements);

  int bubble = 4;
  int out = 999;
  int *optr = &out;
  sp::rec::BTNode<int, keys, cmp> *gt = nullptr;
  auto res = sp::rec::impl::fixup(&tree, &bubble, gt, optr);
  {
    int *out_bubble = std::get<0>(res);
    ASSERT_TRUE(out_bubble);

    sp::rec::BTNode<int, keys, cmp> *right = std::get<1>(res);
    ASSERT_TRUE(right);
    ASSERT_EQ(std::size_t(2), length(right->elements));
    ASSERT_EQ(3, right->elements[0]);
    ASSERT_EQ(4, right->elements[1]);
    delete right;

    ASSERT_EQ(2, *out_bubble);
  }
  ASSERT_EQ(std::size_t(2), length(tree.elements));
  ASSERT_EQ(0, tree.elements[0]);
  ASSERT_EQ(1, tree.elements[1]);
}

TEST(btree_recTest, test_321) {
  sp::rec::BTree<int, 2, sp::greater> tree;
  a_insert(tree, 3);
  { /**/

    ASSERT_EQ(tree.root->elements[0], 3);
  }
  a_insert(tree, 2);
  {
    ASSERT_EQ(tree.root->elements[0], 2);
    ASSERT_EQ(tree.root->elements[1], 3);
  }
  a_insert(tree, 1);
  {
    ASSERT_EQ(tree.root->elements[0], 2);
    {
      ASSERT_EQ(tree.root->children[0]->elements[0], 1);
      ASSERT_EQ(tree.root->children[1]->elements[0], 3);
    }
  }
}

TEST(btree_recTest, test_order3) {
  std::size_t max = 99;
  sp::rec::BTree<int, 2> tree;

  constexpr std::size_t strt = 1;
  for (std::size_t i = strt; i < max; ++i) {
    for (std::size_t a = strt; a < i; ++a) {
      auto res = find(tree, a);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, a);
    }

    for (std::size_t a = i; a < max; ++a) {
      auto res = find(tree, a);
      ASSERT_FALSE(res);
    }

    // printf("insert(%zu)\n", i);
    auto res = a_insert(tree, i);
    // btree::impl::btree::dump(tree.root);
    // printf("=============\n");
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);
  }
}

TEST(btree_recTest, test_order5) {
  // http://www.cs.carleton.edu/faculty/jgoldfea/cs201/spring11/inclass/Tree/BTreefinalNew.pdf
  char raw[] = {'A', 'G', 'F', 'B', 'K', 'D', 'H', 'M', 'J', 'E',
                'S', 'I', 'R', 'X', 'C', 'L', 'N', 'T', 'U', 'P'};
  sp::rec::BTree<char, 4> tree;
  std::size_t max = sizeof(raw);
  for (std::size_t i = 0; i < max; ++i) {
    for (std::size_t a = 0; a < i; ++a) {
      char *res = find(tree, raw[a]);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, raw[a]);
    }

    for (std::size_t a = i; a < max; ++a) {
      char *res = find(tree, raw[a]);
      ASSERT_FALSE(res);
    }
    char *res = insert(tree, raw[i]);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, raw[i]);
    {
      char *res2 = insert(tree, raw[i]);
      ASSERT_TRUE(res2);
      ASSERT_EQ(res2, res);
    }
  }
  btree::impl::btree::dump(tree.root);
}

template <std::size_t values>
static void
btree_rand_insert(prng::xorshift32 &r, sp::Bitset &bset, std::size_t max) {
  sp::rec::BTree<std::size_t, values> tree;
  std::size_t i = 0;
  while (i++ < max) {
    /*insert*/ {
      const std::size_t in(uniform_dist(r, 0, max));
      if (test(bset, in)) {
        auto res = find(tree, in);
        if (!res) {
          printf("not found[%zu]\n", in);
        }
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, in);
      } else {
        ASSERT_FALSE(find(tree, in));
        {
          // printf("insert(tree, in[%zu])\n", in);
          auto p = a_insert(tree, in);
          // btree::impl::btree::dump(tree.root);
          // printf("=============\n");
          ASSERT_TRUE(p);
          ASSERT_EQ(*p, in);
        }

        ASSERT_FALSE(set(bset, std::size_t(in), true));
        {
          auto res = find(tree, in);
          ASSERT_TRUE(res);
          ASSERT_EQ(*res, in);
        }
      }
    }
  } // while

  /*second*/ {
    for_each(bset, [&tree](auto idx, bool set) {
      auto res = find(tree, idx);
      if (set) {
        if (!res) {
          printf("not found[%zu]\n", idx);
        }
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, idx);
      } else {
        ASSERT_FALSE(res);
      }
    });
  }
}

TEST(btree_recTest, test_rand_insert_order3) {
  prng::xorshift32 r(1);
  // while (true) {
  std::uint64_t raw[512] = {0};
  sp::Bitset bset(raw);
  constexpr std::size_t max(sizeof(raw) * 8);
  // std::size_t max = 14;
  btree_rand_insert<2>(r, bset, max);
  // printf("\n\n==============================%u\n", r.state);
  // }
}

TEST(btree_recTest, test_rand_insert_order4) {
  prng::xorshift32 r(1);
  // while (true) {
  std::uint64_t raw[512] = {0};
  sp::Bitset bset(raw);
  constexpr std::size_t max(sizeof(raw) * 8);
  // std::size_t max = 14;
  btree_rand_insert<3>(r, bset, max);
  // printf("\n\n==============================%u\n", r.state);
  // }
}

TEST(btree_recTest, test_rand_insert_order5) {
  prng::xorshift32 r(1);
  // while (true) {
  std::uint64_t raw[512] = {0};
  sp::Bitset bset(raw);
  constexpr std::size_t max(sizeof(raw) * 8);
  // std::size_t max = 14;
  btree_rand_insert<4>(r, bset, max);
  // printf("\n\n==============================%u\n", r.state);
  // }
}

TEST(btree_recTest, test_rand_insert_order6) {
  prng::xorshift32 r(1);
  // while (true) {
  std::uint64_t raw[512] = {0};
  sp::Bitset bset(raw);
  constexpr std::size_t max(sizeof(raw) * 8);
  // std::size_t max = 14;
  btree_rand_insert<5>(r, bset, max);
  // printf("\n\n==============================%u\n", r.state);
  // }
}

TEST(btree_recTest, test_rand_insert_order7) {
  prng::xorshift32 r(1);
  // while (true) {
  std::uint64_t raw[512] = {0};
  sp::Bitset bset(raw);
  constexpr std::size_t max(sizeof(raw) * 8);
  // std::size_t max = 14;
  btree_rand_insert<6>(r, bset, max);
  // printf("\n\n==============================%u\n", r.state);
  // }
}

TEST(btree_recTest, test_rand_insert_order8) {
  prng::xorshift32 r(1);
  // while (true) {
  std::uint64_t raw[512] = {0};
  sp::Bitset bset(raw);
  constexpr std::size_t max(sizeof(raw) * 8);
  // std::size_t max = 14;
  btree_rand_insert<7>(r, bset, max);
  // printf("\n\n==============================%u\n", r.state);
  // }
}

TEST(btree_recTest, test_rand_insert_order9) {
  prng::xorshift32 r(1);
  // while (true) {
  std::uint64_t raw[512] = {0};
  sp::Bitset bset(raw);
  constexpr std::size_t max(sizeof(raw) * 8);
  // std::size_t max = 14;
  btree_rand_insert<8>(r, bset, max);
  // printf("\n\n==============================%u\n", r.state);
  // }
}

TEST(btree_recTest, test_rand_insert_order10) {
  prng::xorshift32 r(1);
  // while (true) {
  std::uint64_t raw[512] = {0};
  sp::Bitset bset(raw);
  constexpr std::size_t max(sizeof(raw) * 8);
  // std::size_t max = 14;
  btree_rand_insert<9>(r, bset, max);
  // printf("\n\n==============================%u\n", r.state);
  // }
}

TEST(btree_recTest, test_rand_insert_order11) {
  prng::xorshift32 r(1);
  // while (true) {
  std::uint64_t raw[512] = {0};
  sp::Bitset bset(raw);
  constexpr std::size_t max(sizeof(raw) * 8);
  // std::size_t max = 14;
  btree_rand_insert<10>(r, bset, max);
  // printf("\n\n==============================%u\n", r.state);
  // }
}

TEST(btree_recTest, test_rand_insert_order12) {
  prng::xorshift32 r(1);
  // while (true) {
  std::uint64_t raw[512] = {0};
  sp::Bitset bset(raw);
  constexpr std::size_t max(sizeof(raw) * 8);
  // std::size_t max = 14;
  btree_rand_insert<11>(r, bset, max);
  // printf("\n\n==============================%u\n", r.state);
  // }
}

TEST(btree_recTest, remove_2_1_3) {
  constexpr std::size_t values = 2;
  sp::rec::BTree<int, values> tree;
  a_insert(tree, 1);
  a_insert(tree, 2);
  a_insert(tree, 3);

  {
    a_remove(tree, 2);
    auto &elements = tree.root->elements;
    ASSERT_EQ(1, elements[0]);
    ASSERT_EQ(std::size_t(2), length(elements));
    ASSERT_EQ(3, elements[1]);
  }
  {
    a_remove(tree, 1);
    auto &elements = tree.root->elements;
    ASSERT_EQ(std::size_t(1), length(elements));
    ASSERT_EQ(3, elements[0]);
  }
  {
    a_remove(tree, 3);
    ASSERT_FALSE(tree.root);
  }
}

TEST(btree_recTest, remove_1_2_3) {
  constexpr std::size_t values = 2;
  sp::rec::BTree<int, values> tree;
  a_insert(tree, 1);
  a_insert(tree, 2);
  a_insert(tree, 3);

  {
    a_remove(tree, 1);
    auto &elements = tree.root->elements;
    ASSERT_EQ(2, elements[0]);
    ASSERT_EQ(std::size_t(2), length(elements));
    ASSERT_EQ(3, elements[1]);
  }
  {
    a_remove(tree, 2);
    auto &elements = tree.root->elements;
    ASSERT_EQ(std::size_t(1), length(elements));
    ASSERT_EQ(3, elements[0]);
  }
  {
    a_remove(tree, 3);
    ASSERT_FALSE(tree.root);
  }
}

TEST(btree_recTest, remove_3_2_1) {
  constexpr std::size_t values = 2;
  sp::rec::BTree<int, values> tree;
  a_insert(tree, 1);
  a_insert(tree, 2);
  a_insert(tree, 3);

  {
    a_remove(tree, 3);
    auto &elements = tree.root->elements;
    ASSERT_EQ(1, elements[0]);
    ASSERT_EQ(std::size_t(2), length(elements));
    ASSERT_EQ(2, elements[1]);
  }
  {
    a_remove(tree, 2);
    auto &elements = tree.root->elements;
    ASSERT_EQ(std::size_t(1), length(elements));
    ASSERT_EQ(1, elements[0]);
  }
  {
    a_remove(tree, 1);
    ASSERT_FALSE(tree.root);
  }
}

template <typename Tree>
static void
btree_seq_remove(int max) {
  const int strt = 6;
  for (int it = strt; it < max; ++it) {
    // printf("=============[%d]\n", it);
    Tree tree;

    // 1. setup
    {
      for (int i = 0; i < it; ++i) {
        a_insert(tree, i);
      }
    }
    // btree::impl::btree::dump(tree.root);

    // 2. remove
    {
      for (int i = 0; i < it; ++i) {
        // 2.1 assert still deleted
        for (int a = 0; a < i; ++a) {
          auto res = find(tree, a);
          ASSERT_FALSE(res);
        }

        // 2.2 not yet deleted
        for (int a = i; a < it; ++a) {
          int *res = find(tree, a);
          ASSERT_TRUE(res);
          ASSERT_EQ(*res, a);
        }

        // 2.3 remove
        a_remove(tree, i);
      }
    }

    ASSERT_FALSE(tree.root);
  }
}

TEST(btree_recTest, remove_order_3) {
  constexpr std::size_t values = 2;
  const int max = 128;
  btree_seq_remove<sp::rec::BTree<int, values>>(max);
}

TEST(btree_recTest, remove_order_4) {
  constexpr std::size_t values = 3;
  const int max = 128;
  btree_seq_remove<sp::rec::BTree<int, values>>(max);
  printf("================\n");
}

TEST(btree_recTest, remove_order_5) {
  constexpr std::size_t values = 4;
  const int max = 128;
  btree_seq_remove<sp::rec::BTree<int, values>>(max);
  printf("================\n");
}

TEST(btree_recTest, remove_order_6) {
  constexpr std::size_t values = 7;
  const int max = 128;
  btree_seq_remove<sp::rec::BTree<int, values>>(max);
  printf("================\n");
}

TEST(btree_recTest, remove_order_7) {
  constexpr std::size_t values = 8;
  const int max = 128;
  btree_seq_remove<sp::rec::BTree<int, values>>(max);
  printf("================\n");
}

TEST(btree_recTest, remove_order_8) {
  constexpr std::size_t values = 9;
  const int max = 128;
  btree_seq_remove<sp::rec::BTree<int, values>>(max);
}

TEST(btree_recTest, remove_order_9) {
  constexpr std::size_t values = 10;
  const int max = 128;
  btree_seq_remove<sp::rec::BTree<int, values>>(max);
}

TEST(btree_recTest, remove_order_10) {
  constexpr std::size_t values = 11;
  const int max = 128;
  btree_seq_remove<sp::rec::BTree<int, values>>(max);
}

TEST(btree_recTest, remove_order_11) {
  constexpr std::size_t values = 12;
  const int max = 128;
  btree_seq_remove<sp::rec::BTree<int, values>>(max);
}

TEST(btree_recTest, remove_order_12) {
  constexpr std::size_t values = 13;
  const int max = 128;
  btree_seq_remove<sp::rec::BTree<int, values>>(max);
}

template <typename Tree>
static void
btree_rand_remove(prng::xorshift32 &r) {
  printf("========seed[%d]\n", r.state);
  Tree tree;
  constexpr std::size_t max = 10;
  sp::StaticArray<int, max> in;
  for (int i = 0; i < max; ++i) {
    int *res = insert(in, i);
    assertx(res);
  }

  shuffle(r, in);
  for (std::size_t i = 0; i < length(in); ++i) {
    const int current = in[i];

    ASSERT_FALSE(find(tree, current));
    {
      int *res = insert(tree, current);
      assertx(res);
    }
    {
      int *res = find(tree, current);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, current);
    }
  }

  shuffle(r, in);
  for (std::size_t i = 0; i < length(in); ++i) {
    const int current = in[i];
    int *res = find(tree, current);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, current);
  }

  shuffle(r, in);
  for (std::size_t i = 0; i < length(in); ++i) {
    btree::impl::btree::dump(tree.root);
    const int current = in[i];
    printf("####remove(tree, %d)\n", current);
    {
      int *res = find(tree, current);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, current);
    }
    ASSERT_TRUE(remove(tree, current));
    ASSERT_FALSE(find(tree, current));
  }

  ASSERT_TRUE(is_empty(tree));
}

TEST(btree_recTest, rand_remove_order_3) {
  constexpr std::size_t values = 2;
  prng::xorshift32 r(1);
  while (true) {
    btree_rand_remove<sp::rec::BTree<int, values>>(r);
  }
}

// TEST(btree_recTest, avl_rec_remove_10) {
//   constexpr std::size_t values = 2;
//   const int max = 128;
//   btree_seq_remove<avl::rec::Tree<int>>(max);
// }
