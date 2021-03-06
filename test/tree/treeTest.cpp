#include <prng/util.h>
#include <prng/xorshift.h>
#include <tree/avl.h>
#include <tree/avl_rec.h>
#include <tree/bst.h>
#include <tree/bst_extra.h>
#include <tree/btree_extra.h>
#include <tree/btree_rec.h>
#include <tree/red-black.h>
#include <util/Bitset.h>

#include <cstring>
#include <gtest/gtest.h>
#include <random>

template <typename T, typename A>
static void
a_insert(avl::rec::Tree<T> &tree, A in) {
  T *p = insert(tree, in);
  ASSERT_TRUE(p);
  ASSERT_EQ(*p, in);

  const bool res = verify(tree);
  if (!res) {
    printf("failed: \n");
    dump(tree);
  }
  ASSERT_TRUE(res);
}

template <typename T, typename A, typename C>
static void
a_insert(binary::Tree<T, C> &tree, A in) {
  std::tuple<T *, bool> tres = insert(tree, in);
  ASSERT_TRUE(std::get<1>(tres));
  int *p = std::get<0>(tres);
  ASSERT_TRUE(p);
  ASSERT_EQ(*p, in);
  ASSERT_TRUE(verify(tree));
}

template <typename T, typename A, typename C>
static void
a_insert(avl::Tree<T, C> &tree, A in) {
  std::tuple<T *, bool> tres = insert(tree, in);
  ASSERT_TRUE(std::get<1>(tres));
  int *p = std::get<0>(tres);
  ASSERT_TRUE(p);
  ASSERT_EQ(*p, in);

  ASSERT_TRUE(verify(tree));
}

template <typename T, typename A, typename C>
static void
a_insert(rb::Tree<T, C> &tree, A in) {
  std::tuple<T *, bool> tres = insert(tree, in);
  ASSERT_TRUE(std::get<1>(tres));
  int *p = std::get<0>(tres);
  ASSERT_TRUE(p);
  ASSERT_EQ(*p, in);

  ASSERT_TRUE(verify(tree));
}

template <typename T, typename C, std::size_t keys>
bool
verify(const sp::rec::BTree<T, keys, C> &) {
  // dummy
  return true;
}

template <typename T, typename C, std::size_t keys>
void
dump(const sp::rec::BTree<T, keys, C> &tree) {
  btree::impl::btree::dump(tree.root);
}

template <typename T, typename C, std::size_t keys, typename A>
static void
a_insert(sp::rec::BTree<T, keys, C> &tree, A in) {
  // printf("insert(tree, %d)\n", in);
  T *p = insert(tree, in);
  ASSERT_TRUE(p);
  ASSERT_EQ(*p, in);

  ASSERT_TRUE(verify(tree));
}

template <class Tree_t, typename T, std::size_t in_size>
static void
find_stuff(Tree_t &tree, std::size_t deleted, T (&in)[in_size]) {
  for (std::size_t k = 0; k < in_size; ++k) {
    auto *f = find(tree, in[k]);
    if (k < deleted) {
      ASSERT_TRUE(f == nullptr);
    } else {
      if (!f) {
        printf("%p = find(tree, %d)\n", f, in[k]);
        printf("find|\n");
        dump(tree);
      }
      ASSERT_TRUE(f);
      ASSERT_TRUE(*f == in[k]);
    }
  }
}

template <class Tree_t>
static void
random_insert_delete(std::size_t goal) {
  std::size_t counter = 0;
  // std::random_device rd;
  // std::mt19937 g(rd());
  std::mt19937 g(0);

  while (counter < goal) {
    if (counter > 0 && counter % 100 == 0) {
      printf("cnt: %zu\n", counter);
    }
    Tree_t tree;
    constexpr int in_size = 200;
    int in[in_size];
    for (int i = 0; i < in_size; ++i) {
      in[i] = i;
    }

    std::shuffle(in, in + in_size, g);
    for (int i = 0; i < in_size; ++i) {
      for (int k = 0; k < i; ++k) {
        auto f = find(tree, in[k]);
        ASSERT_TRUE(f);
        ASSERT_TRUE(*f == in[k]);
      }
      // printf(".%d <- %d\n", i, in[i]);
      a_insert(tree, in[i]);
      {
        int *const fptr = find(tree, in[i]);
        ASSERT_TRUE(fptr);
        ASSERT_TRUE(*fptr == in[i]);
      }

      if (!verify(tree)) {
        dump(tree);
        ASSERT_TRUE(false);
      }
    }
    // dump(tree, "");

    std::shuffle(in, in + in_size, g);
    for (int i = 0; i < in_size; ++i) {
      find_stuff(tree, i, in);

      // dump(tree, "before|");
      // printf("--\n");
      // printf("remove(tree,%d)\n", in[i]);
      bool rb = remove(tree, in[i]);
      // printf(" = %s\n", rb ? "true" : "false");
      ASSERT_TRUE(rb);
      if (!verify(tree)) {
        printf("\n");
        dump(tree);
        ASSERT_TRUE(false);
      } else {
        // dump(tree, "rem|");
      }

      find_stuff(tree, i + 1, in);
      // printf("---------------\n");
    }
    counter++;
  }
  // TODO insert duplicate
  // TODO delete non existing

  printf("done\n");
}

template <class Tree_t>
static void
random_insert(std::size_t goal) {
  std::random_device rd;
  std::mt19937 g(rd());
  std::size_t counter = 0;
  while (counter < goal) {
    if (counter > 0 && counter % 100 == 0) {
      printf("cnt: %zu\n", counter);
    }
    Tree_t tree;
    constexpr int in_size = 1000;
    int in[in_size];
    for (int i = 1; i <= in_size; ++i) {
      in[i - 1] = i;
    }

    std::shuffle(in, in + in_size, g);
    for (int i = 0; i < in_size; ++i) {
      for (int k = 0; k < i; ++k) {
        const int *f = find(tree, in[k]);
        ASSERT_TRUE(f);
        ASSERT_TRUE(*f == in[k]);
      }
      // printf(".%d <- ", i);
      a_insert(tree, in[i]);

      const int *const fptr = find(tree, in[i]);
      if (!fptr) {
        dump(tree);
      }
      ASSERT_TRUE(fptr);
      ASSERT_TRUE(*fptr == in[i]);

      if (!verify(tree)) {
        dump(tree);
        ASSERT_TRUE(false);
      }
    }

    counter++;
  }
  // printf("done\n");
}

TEST(treeTest, test_insert_delete_bst) {
  random_insert_delete<binary::Tree<int>>(10);
}

TEST(treeTest, test_insert_bst) {
  random_insert<binary::Tree<int>>(10);
}

TEST(treeTest, test_insert_avl) {
  random_insert<avl::Tree<int>>(10);
}

TEST(treeTest, test_inser_remove_avl) {
  random_insert_delete<avl::Tree<int>>(10);
}

TEST(treeTest, test_insert_avl_rec) {
  random_insert<avl::rec::Tree<int>>(10);
}

TEST(treeTest, test_inser_remove_avl_rec) {
  random_insert_delete<avl::rec::Tree<int>>(10);
}

TEST(treeTest, test_insert_remove_red_black) {
  // random_insert_delete<rb::Tree>(1000);
}

TEST(treeTest, test_insert_red_black) {
  random_insert<rb::Tree<int>>(10);
}

TEST(treeTest, test_insert_btree_rec_order3) {
  random_insert<sp::rec::BTree<int, 2>>(10);
}

TEST(treeTest, test_inser_remove_btree_rec_order3) {
  random_insert_delete<sp::rec::BTree<int, 2>>(10);
}

// TEST(treeTest, test_insert_StaticTree) {
//   random_insert<bst::StaticTree<int>>(10);
// }
template <class Tree_t, std::size_t cap = 256>
static void
random_insert_random_delete() {
  prng::xorshift32 r(5);
  std::uint64_t raw[cap] = {0};
  sp::Bitset bset(raw);
  constexpr std::uint32_t max(sizeof(raw) * 8);
  std::size_t i = 0;
  Tree_t tree;
  std::size_t balance = 0;
  while (i++ < max) {
    {
      const auto in = uniform_dist(r, 0, max);
      if (test(bset, std::size_t(in))) {
        auto res = find(tree, in);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, std::size_t(in));
      } else {
        {
          auto res = find(tree, in);
          ASSERT_FALSE(res);
        }
        { a_insert(tree, in); }
        ++balance;

        ASSERT_FALSE(set(bset, std::size_t(in), true));
        {
          auto res = find(tree, in);
          if (!res) {
            dump(tree);
          }
          ASSERT_TRUE(res);
          ASSERT_EQ(*res, in);
        }
      }
    }

    {
      const auto del = uniform_dist(r, 0, 10);
      for (std::size_t x = 0; x < del; ++x) {
        const auto in = uniform_dist(r, 0, max);
        if (test(bset, std::size_t(in))) {
          {
            auto res = find(tree, in);
            ASSERT_TRUE(res);
            ASSERT_EQ(*res, in);
          }

          // if (in == 9) {
          //   printf("\n\n========\n");
          //   printf("before: remove(%d)\n", in);
          //   dump(tree);
          // }
          ASSERT_TRUE(remove(tree, in));
          const bool v = verify(tree);
          if (!v) {
            printf("failed: remove(%d)\n", in);
            dump(tree);
          }
          ASSERT_TRUE(v);
          ASSERT_TRUE(set(bset, std::size_t(in), false));
          {
            auto res = find(tree, in);
            ASSERT_FALSE(res);
          }
          ASSERT_TRUE(balance > 0);
          --balance;
        } else {
          {
            auto res = find(tree, in);
            ASSERT_FALSE(res);
          }
          ASSERT_FALSE(remove(tree, in));
        }
      }
    }
  }
  for_each(bset, [&tree, &balance](auto idx, bool set) {
    // printf("idx[%zu]\n", idx);
    if (set) {
      {
        auto res = find(tree, idx);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, idx);
      }
      {
        ASSERT_TRUE(remove(tree, idx));
        ASSERT_TRUE(verify(tree));
        ASSERT_TRUE(balance > 0);
        --balance;
      }
      {
        auto res = find(tree, idx);
        ASSERT_FALSE(res);
      }
    } else {
      { ASSERT_FALSE(remove(tree, idx)); }
      const bool v = verify(tree);
      if (!v) {
        dump(tree);
      }
      ASSERT_TRUE(v);
      {
        auto res = find(tree, idx);
        ASSERT_FALSE(res);
      }
    }
  });

  ASSERT_EQ(std::size_t(0), balance);
  ASSERT_TRUE(is_empty(tree));
}

TEST(treeTest, test_rand_ins_rand_remove_bst) {
  random_insert_random_delete<binary::Tree<int>>();
}

TEST(treeTest, test_rand_ins_rand_remove_avl) {
  random_insert_random_delete<avl::Tree<int>>();
}

TEST(treeTest, test_rand_ins_rand_remove_rec_avl) {
  random_insert_random_delete<avl::rec::Tree<int>>();
}

TEST(treeTest, test_rand_ins_rand_remove_rec_btree_order3) {
  random_insert_random_delete<sp::rec::BTree<int, 2>>();
}

TEST(treeTest, test_rand_ins_rand_remove_rec_btree_order4) {
  random_insert_random_delete<sp::rec::BTree<int, 3>>();
}

TEST(treeTest, test_rand_ins_rand_remove_rec_btree_order5) {
  random_insert_random_delete<sp::rec::BTree<int, 4>>();
}

TEST(treeTest, test_rand_ins_rand_remove_rec_btree_order6) {
  random_insert_random_delete<sp::rec::BTree<int, 5>>();
}

TEST(treeTest, test_rand_ins_rand_remove_rec_btree_order7) {
  random_insert_random_delete<sp::rec::BTree<int, 6>>();
}

TEST(treeTest, test_rand_ins_rand_remove_rec_btree_order8) {
  random_insert_random_delete<sp::rec::BTree<int, 7>>();
}

TEST(treeTest, test_rand_ins_rand_remove_rec_btree_order9) {
  random_insert_random_delete<sp::rec::BTree<int, 8>>();
}

TEST(treeTest, test_rand_ins_rand_remove_rec_btree_order10) {
  random_insert_random_delete<sp::rec::BTree<int, 9>>();
}

TEST(treeTest, test_rand_ins_rand_remove_rec_btree_order11) {
  random_insert_random_delete<sp::rec::BTree<int, 10>>();
}

TEST(treeTest, test_rand_ins_rand_remove_rec_btree_order12) {
  random_insert_random_delete<sp::rec::BTree<int, 11>>();
}
