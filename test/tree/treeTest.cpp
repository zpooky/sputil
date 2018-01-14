#include <tree/bst.h>
#include <tree/avl.h>
#include <tree/red-black.h>
#include <tree/avl_insert.h>
#include <tree/avl_remove.h>

#include "gtest/gtest.h"
#include <random>

template <class Tree_t, typename T, std::size_t in_size>
static void
find_stuff(Tree_t &tree, std::size_t deleted, T (&in)[in_size]) {
  for (int k = 0; k < in_size; ++k) {
    auto *f = sp::find(tree, in[k]);
    if (k < deleted) {
      assert(f == nullptr);
    } else {
      if (!f) {
        printf("%p = sp::find(tree, %d)\n", f, in[k]);
        dump(tree, "find|");
      }
      assert(f);
      assert(*f == in[k]);
    }
  }
}

template <template <typename> class Tree_t>
static void
test_tree(std::size_t goal) {
  std::size_t counter = 0;
  // std::random_device rd;
  // std::mt19937 g(rd());
  std::mt19937 g(0);

  while (counter < goal) {
    if (counter % 10 == 0) {
      printf("cnt: %zu\n", counter);
    }
    Tree_t<int> tree;
    constexpr int in_size = 10;
    int in[in_size];
    for (int i = 0; i < in_size; ++i) {
      in[i] = i;
    }

    std::shuffle(in, in + in_size, g);
    for (int i = 0; i < in_size; ++i) {
      for (int k = 0; k < i; ++k) {
        auto f = sp::find(tree, in[k]);
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
        int *const fptr = sp::find(tree, in[i]);
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

      dump(tree, "before|");
      printf("--\n");
      printf("remove(tree,%d)\n", in[i]);
      bool rb = remove(tree, in[i]);
      printf(" = %s\n", rb ? "true" : "false");
      assert(rb);
      if (!verify(tree)) {
        printf("\n");
        dump(tree, "rem|");
        assert(false);
      } else {
        dump(tree, "rem|");
      }

      find_stuff(tree, i + 1, in);
    }
    counter++;
  }
  // TODO insert duplicate
  // TODO delete non existing

  printf("done\n");
}

TEST(treeTest, test_bst) {
  // test_tree<bst::Tree>(10000);
}

TEST(treeTest, test_avl) {
  // TODO
  // test_tree<avl::Tree>(1000);
}

TEST(treeTest, test_red_black) {
  // test_tree<rb::Tree>(1000);
}
