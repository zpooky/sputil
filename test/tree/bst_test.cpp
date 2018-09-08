#include <gtest/gtest.h>

#include <cstdio>
#include <tree/bst.h>
#include <tree/bst_extra.h>

TEST(bst_test, test_eq) {
  binary::Tree<int> t;
  binary::Tree<int> t4;
  for (int i = 0; i < 10; ++i) {
    ASSERT_TRUE(binary::rec::equal(t, t));
    ASSERT_TRUE(std::get<1>(insert(t, i)));
    ASSERT_TRUE(binary::rec::is_binary_tree_bst(t));

    ASSERT_TRUE(std::get<1>(insert(t4, i)));
    ASSERT_TRUE(binary::rec::equal(t4, t));
    ASSERT_TRUE(binary::rec::is_binary_tree_bst(t4));
  }

  binary::Tree<int> t3;
  for (int i = 0; i < 9; ++i) {
    ASSERT_FALSE(binary::rec::equal(t3, t));
    ASSERT_TRUE(std::get<1>(insert(t3, i)));
  }

  binary::Tree<int> t2;
  ASSERT_FALSE(binary::rec::equal(t, t2));
}

TEST(bst_test, test_height_and_other) {
  {
    binary::Tree<int> t;
    ASSERT_EQ(std::size_t(0), binary::rec::height(t));
    for (std::size_t i = 0; i < 10; ++i) {
      ASSERT_EQ(i, binary::rec::height(t));
      ASSERT_TRUE(std::get<1>(insert(t, int(i))));
    }
  }
  {
    binary::Tree<int> t;
    ASSERT_EQ(std::size_t(0), binary::rec::max_width(t));
    ASSERT_EQ(std::size_t(0), binary::rec::height(t));
    ASSERT_TRUE(std::get<1>(insert(t, 5)));
    ASSERT_EQ(std::size_t(0), binary::rec::max_width(t));
    ASSERT_EQ(std::size_t(1), binary::rec::height(t));
    {
      ASSERT_TRUE(std::get<1>(insert(t, 2)));
      ASSERT_EQ(std::size_t(2), binary::rec::max_width(t));
      ASSERT_EQ(std::size_t(2), binary::rec::height(t));
      ASSERT_TRUE(std::get<1>(insert(t, 9)));
      ASSERT_EQ(std::size_t(2), binary::rec::max_width(t));
      ASSERT_EQ(std::size_t(2), binary::rec::height(t));
    }
    {
      ASSERT_TRUE(std::get<1>(insert(t, 4)));
      ASSERT_EQ(std::size_t(4), binary::rec::max_width(t));
      ASSERT_EQ(std::size_t(3), binary::rec::height(t));
      ASSERT_TRUE(std::get<1>(insert(t, 1)));
      ASSERT_EQ(std::size_t(4), binary::rec::max_width(t));
      ASSERT_EQ(std::size_t(3), binary::rec::height(t));

      ASSERT_TRUE(std::get<1>(insert(t, 15)));
      ASSERT_EQ(std::size_t(4), binary::rec::max_width(t));
      ASSERT_EQ(std::size_t(3), binary::rec::height(t));
      ASSERT_TRUE(std::get<1>(insert(t, 8)));
      ASSERT_EQ(std::size_t(4), binary::rec::max_width(t));
      ASSERT_EQ(std::size_t(3), binary::rec::height(t));
    }
    ASSERT_TRUE(std::get<1>(insert(t, 20)));
    ASSERT_EQ(std::size_t(8), binary::rec::max_width(t));
    ASSERT_EQ(std::size_t(4), binary::rec::height(t));

    dump(t);
    printf("inorder: \n[");
    binary::rec::inorder(t, [](const int &c) { printf("%d, ", c); });
    printf("]\n");

    printf("preorder: \n[");
    binary::rec::preorder(t, [](const int &c) { printf("%d, ", c); });
    printf("]\n");

    printf("postorder: \n[");
    binary::rec::postorder(t, [](const int &c) { printf("%d, ", c); });
    printf("]\n");

    printf("rec levelorder: \n[");
    binary::rec::levelorder(t, [](const int &c) { printf("%d, ", c); });
    printf("]\n");

    printf("it levelorder: \n[");
    binary::it::levelorder(t, [](const int &c) { printf("%d, ", c); });
    printf("]\n");

    printf("rec reverse-levelorder(not working): \n[");
    binary::rec::reverse_levelorder(t, [](const int &c) { printf("%d, ", c); });
    printf("]\n");

    printf("it reverse-levelorder: \n[");
    binary::it::reverse_levelorder(t, [](const int &c) { printf("%d, ", c); });
    printf("]\n");

    {
      ASSERT_TRUE(std::get<1>(insert(t, 10)));
      ASSERT_EQ(std::size_t(4), binary::rec::height(t));
    }
    dump(t);

    binary::rec::delete_self(t);
    ASSERT_EQ(t.root, nullptr);
  }
}
