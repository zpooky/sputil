#include <gtest/gtest.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <tree/btree_extra.h>
#include <tree/btree_rec.h>
#include <util/Bitset.h>

template <typename T, std::size_t n, typename Cmp, typename V>
static T *
a_insert(sp::rec::BTree<T, n, Cmp> &tree, V v) {
  printf("insert(%d)\n", v);
  T *res = insert(tree, v);
  btree::impl::btree::dump(tree.root);
  printf("=============\n");
  assertx(res);
  assertxs(*res == v, *res, v);
  {
    T *res2 = find(tree, v);
    assertx(res2);
    assertxs(*res2 == v, *res2, v);
    assertxs(res2 == res, *res2, *res);
  }
  {
    T *res2 = insert(tree, v);
    assertx(res2);
    assertxs(*res2 == v, *res2, v);
    assertxs(res2 == res, *res2, *res);
  }
  {
    T *res2 = find(tree, v);
    assertx(res2);
    assertxs(*res2 == v, *res2, v);
    assertxs(res2 == res, *res2, *res);
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
  constexpr std::size_t max = 20;
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
  constexpr std::size_t max = 20;
  sp::UinStaticArray<int, max> elements;
  sp::bin_insert<int, max, int, cmp>(elements, 3);
  sp::bin_insert<int, max, int, cmp>(elements, 2);
  dump(elements);

  int extra = 1;
  const int *res = sp::rec::impl::median<int, max, cmp>(elements, &extra);
  ASSERT_TRUE(res);
  ASSERT_EQ(*res, 2);
}

TEST(btree_recTest, test_order321) {
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
      ASSERT_EQ(tree.root->children[1]->elements[1], 3);
    }
  }
}

TEST(btree_recTest, test_order3) {
  std::size_t max = 99;
  sp::rec::BTree<std::size_t, 2> tree;

  constexpr std::size_t strt = 1;
  for (std::size_t i = strt; i < max; ++i) {
    for (std::size_t a = strt; a < i; ++a) {
      std::size_t *res = find(tree, a);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, a);
    }

    for (std::size_t a = i; a < max; ++a) {
      std::size_t *res = find(tree, a);
      ASSERT_FALSE(res);
    }

    printf("insert(%zu)\n", i);
    std::size_t *res = insert(tree, i);
    btree::impl::btree::dump(tree.root);
    printf("=============\n");
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);
    {
      std::size_t *res2 = insert(tree, i);
      ASSERT_TRUE(res2);
      ASSERT_EQ(res, res2);
      ASSERT_EQ(i, *res2);
    }
  }
}

TEST(btree_recTest, test_order5) {
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

TEST(btree_recTest, test_rand) {
  prng::xorshift32 r(5);
  std::uint64_t raw[256] = {0};
  sp::Bitset bset(raw);
  constexpr std::uint32_t max(sizeof(raw) * 8);
  sp::rec::BTree<std::size_t, 2> tree;
  std::size_t i = 0;
  while (i++ < max) {
    const auto in = uniform_dist(r, 0, max);
    if (test(bset, std::size_t(in))) {
      auto res = find(tree, in);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in);
    } else {
      ASSERT_FALSE(find(tree, in));
      {
        printf("insert(tree, in[%zu])\n", in);
        auto p = insert(tree, in);
        btree::impl::btree::dump(tree.root);
        printf("=============\n");
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

  for_each(bset, [&tree](auto idx, bool set) {
    auto res = find(tree, idx);
    if (set) {
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, idx);
    } else {
      auto res = find(tree, idx);
      ASSERT_FALSE(res);
    }
  });
}
