#include <tree/avl.h>
#include <tree/bst.h>
#include <tree/red-black.h>

#include "gtest/gtest.h"
#include <cstring>
#include <random>

template <class Tree_t, typename T, std::size_t in_size>
static void
find_stuff(Tree_t &tree, std::size_t deleted, T (&in)[in_size]) {
  for (std::size_t k = 0; k < in_size; ++k) {
    auto *f = find(tree, in[k]);
    if (k < deleted) {
      assert(f == nullptr);
    } else {
      if (!f) {
        printf("%p = find(tree, %d)\n", f, in[k]);
        dump(tree, "find|");
      }
      assert(f);
      assert(*f == in[k]);
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
    if (counter > 0 && counter % 10 == 0) {
      printf("cnt: %zu\n", counter);
    }
    Tree_t tree;
    constexpr int in_size = 1000;
    int in[in_size];
    for (int i = 0; i < in_size; ++i) {
      in[i] = i;
    }

    std::shuffle(in, in + in_size, g);
    for (int i = 0; i < in_size; ++i) {
      for (int k = 0; k < i; ++k) {
        auto f = find(tree, in[k]);
        assert(f);
        assert(*f == in[k]);
      }
      // printf(".%d <- %d\n", i, in[i]);
      auto res = insert(tree, in[i]);
      int *const iptr = std::get<0>(res);
      assert(std::get<1>(res) == true);
      assert(iptr);
      assert(*iptr == in[i]);
      {
        int *const fptr = find(tree, in[i]);
        assert(fptr);
        assert(fptr == iptr);
        assert(*fptr == *iptr);
      }

      if (!verify(tree)) {
        dump(tree, "ins|");
        assert(false);
      }
    }

    std::shuffle(in, in + in_size, g);
    for (int i = 0; i < in_size; ++i) {
      find_stuff(tree, i, in);

      // dump(tree, "before|");
      // printf("--\n");
      // printf("remove(tree,%d)\n", in[i]);
      bool rb = remove(tree, in[i]);
      // printf(" = %s\n", rb ? "true" : "false");
      assert(rb);
      if (!verify(tree)) {
        printf("\n");
        dump(tree, "rem|");
        assert(false);
      } else {
        // dump(tree, "rem|");
      }

      find_stuff(tree, i + 1, in);
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
      auto res = insert(tree, in[i]);
      int *const iptr = std::get<0>(res);
      ASSERT_TRUE(std::get<1>(res) == true);
      ASSERT_TRUE(iptr);
      ASSERT_TRUE(*iptr == in[i]);

      const int *const fptr = find(tree, in[i]);
      if(!fptr){
        dump(tree);
      }
      ASSERT_TRUE(fptr);
      ASSERT_TRUE(fptr == iptr);
      ASSERT_TRUE(*fptr == *iptr);

      if (!verify(tree)) {
        dump(tree, "after|");
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
  // TODO
  // random_insert_delete<avl::Tree<int>>(1000);
}

TEST(treeTest, test_insert_remove_red_black) {
  // random_insert_delete<rb::Tree>(1000);
}

TEST(treeTest, test_insert_red_black) {
  random_insert<rb::Tree<int>>(10);
}

// TEST(treeTest, test_insert_StaticTree) {
//   random_insert<bst::StaticTree<int>>(10);
// }


