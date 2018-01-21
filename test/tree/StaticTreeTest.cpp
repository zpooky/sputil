#include <tree/StaticTree.h>

#include "gtest/gtest.h"
#include <cstring>
#include <random>

// static void
// print_arr(int *buff, std::size_t SIZE) {
//   for (std::size_t i = 0; i < SIZE; ++i) {
//     printf("%d.", buff[i]);
//   }
//   printf("\n");
// }

#define assert_insert(tree, v, present)                                        \
  do {                                                                         \
    auto res = insert(tree, v);                                                \
    int *const iptr = std::get<0>(res);                                        \
    ASSERT_EQ(std::get<1>(res), present);                                      \
    if (present) {                                                             \
      ASSERT_TRUE(iptr);                                                       \
    } else {                                                                   \
      ASSERT_TRUE(!iptr);                                                      \
    }                                                                          \
  } while (0)

struct IntPresent {
  constexpr bool
  operator()(int i) const {
    return i != -1;
  }
};

TEST(StaticTreeTest, test_static_tree_swap) {
  int i = 0;
  int a = 1;
  std::swap(i, a);
  ASSERT_EQ(a, 0);
  ASSERT_EQ(i, 1);
}

// TEST(StaticTreeTest, test_remove_StaticTree) {
//   std::mt19937 g(0);
//
//   constexpr std::size_t in_size = 7;
//   for (std::size_t a = 0; a < 1000; ++a) {
//     printf("=============================\n");
//     printf("=============================\n");
//     printf("=============================\n");
//     int buffer[in_size];
//     std::memset(buffer, -1, sizeof(buffer));
//     bst::StaticTree<int, sp::greater, IntPresent> tree(buffer);
//
//     int in[] = {4, 2, 3, 1, 6, 5, 7};
//     for (std::size_t i = 0; i < in_size; ++i) {
//       assert_insert(tree, in[i], true);
//     }
//     printf("%zu.\n", a);
//     std::shuffle(in, in + in_size, g);
//     for (std::size_t i = 0; i < in_size; ++i) {
//       dump(tree);
//       print_arr(buffer, in_size);
//       printf("remove(%d)\n", in[i]);
//       ASSERT_TRUE(remove(tree, in[i]));
//       printf("-------------\n");
//     }
//   }
// }

TEST(StaticTreeTest, test_insert_optimal_StaticTree) {
  constexpr std::size_t size = 7;
  int buffer[size];
  std::memset(buffer, -1, sizeof(buffer));

  bst::StaticTree<int, sp::greater, IntPresent> tree(buffer);

  assert_insert(tree, 4, true);

  assert_insert(tree, 2, true);
  assert_insert(tree, 3, true);
  assert_insert(tree, 1, true);

  assert_insert(tree, 6, true);
  assert_insert(tree, 5, true);
  assert_insert(tree, 7, true);

  for (std::size_t i = 0; i < size; ++i) {
    ASSERT_FALSE(buffer[i] == -1);

    const int *r = bst::find(tree, buffer[i]);
    ASSERT_TRUE(r != nullptr);
    ASSERT_EQ(*r, buffer[i]);
  }

  ASSERT_FALSE(bst::find(tree, -1));
  ASSERT_FALSE(bst::find(tree, 0));
  ASSERT_FALSE(bst::find(tree, 8));
  ASSERT_FALSE(bst::find(tree, 9));

  for (int i = size + 1; i < 1024; ++i) {
    assert_insert(tree, i, false);
  }

  // for (int i = 1; i <= size; ++i) {
  //   for (int k = i; k <= size; ++k) {
  //     const int *r = bst::find(tree, k);
  //     printf("find(%d): %d\n", k, r != nullptr);
  //     ASSERT_TRUE(r != nullptr);
  //     ASSERT_EQ(*r, k);
  //   }
  //
  //   bool res = remove(tree, i);
  //   printf("remove(%d): %d\n", i, res);
  //   ASSERT_TRUE(res);
  //   ASSERT_FALSE(remove(tree, i));
  //   print_arr(tree.buffer, tree.capacity);
  //   for (int k = 1; k <= i; ++k) {
  //     const int *r = bst::find(tree, k);
  //     printf("find(%d): %d \n", k, r != nullptr);
  //     ASSERT_TRUE(r == nullptr);
  //   }
  //
  //   for (int k = i + 1; k <= size; ++k) {
  //     const int *r = bst::find(tree, k);
  //     printf("find(%d): %d\n", k, r != nullptr);
  //     ASSERT_TRUE(r != nullptr);
  //     ASSERT_EQ(*r, k);
  //   }
  //   printf("---\n");
  //
  //   {
  //     for (std::size_t a = 0; a < size; ++a) {
  //
  //       if (buffer[a] != -1) {
  //         const int *r = bst::find(tree, buffer[a]);
  //         ASSERT_TRUE(r != nullptr);
  //         ASSERT_EQ(*r, buffer[a]);
  //       }
  //     }
  //   }
  // }
}

TEST(StaticTreeTest, test_for_each) {
  constexpr std::size_t size = 7;
  int buffer[size];
  std::memset(buffer, -1, sizeof(buffer));

  bst::StaticTree<int, sp::greater, IntPresent> tree(buffer);
  int i = 0;

  bst::in_order_for_each(tree, [&i](auto &node) {
    ASSERT_EQ(node, -1);
    node = i++;
    //
  });
  ASSERT_EQ(std::size_t(i), tree.capacity);
  {
    int cmp = 0;
    bst::in_order_for_each(tree, [&cmp](auto node) {
      assert(node == cmp);
      cmp++;
    });
    ASSERT_EQ(cmp, i);
    ASSERT_EQ(std::size_t(cmp), tree.capacity);
    // printf("cmp: %d\n", cmp);
  }
  // {
  //   printf("in_order: ");
  //   for (std::size_t i = 0; i < Type::capacity; ++i) {
  //     printf("%d,", tree.buffer[i].data);
  //   }
  //   printf("\n");
  // }
  for (int k = 0; k < int(tree.capacity); ++k) {
    auto *r = find(tree, k);
    ASSERT_TRUE(r);
    ASSERT_TRUE(*r == k);
  }
}
