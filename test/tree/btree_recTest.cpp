#include <gtest/gtest.h>
#include <tree/btree_rec.h>

TEST(btree_recTest, test_shape) {
  std::size_t max = 1024;
  sp::rec::BTree<std::size_t, 4> tree;

  for (std::size_t i = 0; i < max; ++i) {
    for (std::size_t a = 0; a < i; ++a) {
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
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);
  }
}
