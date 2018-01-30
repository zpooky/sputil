#include <tree/avl.h>

#include "gtest/gtest.h"

#include <random>

struct AVLData {
  using T = int;
  bool present;
  T data;
  AVLData()
      : present(false)
      , data(0) {
  }
  explicit AVLData(T d)
      : present(true)
      , data(d) {
  }

  explicit operator bool() const noexcept {
    return present;
  }

  AVLData &
  operator=(const T &o) noexcept {
    present = true;
    data = o;
    return *this;
  }

  operator T() const noexcept {
    return data;
  }

  bool
  operator==(const T &o) const noexcept {
    return data == o;
  }

  bool
  operator==(const AVLData &o) const noexcept {
    return operator==(o.data);
  }

  bool
  operator>(const T &o) const noexcept {
    return data > o;
  }

  bool
  operator>(const AVLData &o) const noexcept {
    return operator>(o.data);
  }

  bool
  operator<(const T &o) const noexcept {
    return data < o;
  }

  bool
  operator<(const AVLData &o) const noexcept {
    return operator<(o.data);
  }

  int
  cmp(const int &o) const noexcept {
    if (data == o) {
      return 0;
    }
    if (data > o) {
      // TODO wrong?
      return -1;
    }
    return 1;
  }

  int
  cmp(const AVLData &o) const noexcept {
    return cmp(o.data);
  }
};

TEST(avlTest, rotate_left) {
  auto a = new avl::Node<AVLData>(AVLData(1));
  a->balance = 2;
  auto b = a->right = new avl::Node<AVLData>(AVLData(2), a);
  b->balance = 1;
  auto c = b->right = new avl::Node<AVLData>(AVLData(3), b);
  c->balance = 0;

  auto reb = avl::impl::avl::rotate_left(a);
  ASSERT_TRUE(reb->value == b->value);
  ASSERT_TRUE(reb->parent == nullptr);
  ASSERT_TRUE(reb->balance == 0);

  ASSERT_TRUE(reb->left->value == a->value);
  ASSERT_TRUE(reb->left->parent == b);
  ASSERT_TRUE(reb->left->left == nullptr);
  ASSERT_TRUE(reb->left->right == nullptr);
  ASSERT_TRUE(reb->left->balance == 0);

  ASSERT_TRUE(reb->right->value == c->value);
  ASSERT_TRUE(reb->right->parent == b);
  ASSERT_TRUE(reb->right->left == nullptr);
  ASSERT_TRUE(reb->right->right == nullptr);
  ASSERT_TRUE(reb->right->balance == 0);
}

TEST(avlTest, rotate_right) {
  auto c = new avl::Node<AVLData>(AVLData(3));
  c->balance = -2;
  auto b = c->left = new avl::Node<AVLData>(AVLData(2), c);
  b->balance = -1;
  auto a = b->left = new avl::Node<AVLData>(AVLData(1), b);
  a->balance = 0;

  auto reb = avl::impl::avl::rotate_right(c);
  ASSERT_TRUE(reb->value == b->value);
  ASSERT_TRUE(reb->parent == nullptr);
  ASSERT_TRUE(reb->balance == 0);

  ASSERT_TRUE(reb->left->value == a->value);
  ASSERT_TRUE(reb->left->parent == b);
  ASSERT_TRUE(reb->left->left == nullptr);
  ASSERT_TRUE(reb->left->right == nullptr);
  ASSERT_TRUE(reb->left->balance == 0);

  ASSERT_TRUE(reb->right->value == c->value);
  ASSERT_TRUE(reb->right->parent == b);
  ASSERT_TRUE(reb->right->left == nullptr);
  ASSERT_TRUE(reb->right->right == nullptr);
  ASSERT_TRUE(reb->right->balance == 0);
}

TEST(avlTest, double_rotate_left) {
  auto c = new avl::Node<AVLData>(AVLData(3));
  c->balance = -2;
  auto b = c->left = new avl::Node<AVLData>(AVLData(1), c);
  b->balance = 1;
  auto a = b->right = new avl::Node<AVLData>(AVLData(2), b);
  a->balance = 0;

  c->left = avl::impl::avl::rotate_left(c->left);
  auto reb = avl::impl::avl::rotate_right(c);
  ASSERT_TRUE(reb->value == 2);
  ASSERT_TRUE(reb->parent == nullptr);
  ASSERT_TRUE(reb->balance == 0);

  ASSERT_TRUE(reb->left->value == 1);
  ASSERT_TRUE(reb->left->parent);
  ASSERT_TRUE(reb->left->parent == a);
  ASSERT_TRUE(reb->left->left == nullptr);
  ASSERT_TRUE(reb->left->right == nullptr);
  ASSERT_TRUE(reb->left->balance == 0);

  ASSERT_TRUE(reb->right->value == 3);
  ASSERT_TRUE(reb->right->parent);
  ASSERT_TRUE(reb->right->parent == a);
  ASSERT_TRUE(reb->right->left == nullptr);
  ASSERT_TRUE(reb->right->right == nullptr);
  ASSERT_TRUE(reb->right->balance == 0);
}

TEST(avlTest, double_rotate_right) {
  auto c = new avl::Node<AVLData>(AVLData(1));
  c->balance = 2;
  auto b = c->right = new avl::Node<AVLData>(AVLData(3), c);
  b->balance = -1;
  auto a = b->left = new avl::Node<AVLData>(AVLData(2), b);
  a->balance = 0;

  c->right = avl::impl::avl::rotate_right(c->right);
  auto reb = avl::impl::avl::rotate_left(c);
  ASSERT_TRUE(reb->value == 2);
  ASSERT_TRUE(reb->parent == nullptr);
  ASSERT_TRUE(reb->balance == 0);

  ASSERT_TRUE(reb->left->value == 1);
  ASSERT_TRUE(reb->left->parent);
  ASSERT_TRUE(reb->left->parent == a);
  ASSERT_TRUE(reb->left->left == nullptr);
  ASSERT_TRUE(reb->left->right == nullptr);
  ASSERT_TRUE(reb->left->balance == 0);

  ASSERT_TRUE(reb->right->value == 3);
  ASSERT_TRUE(reb->right->parent);
  ASSERT_TRUE(reb->right->parent == a);
  ASSERT_TRUE(reb->right->left == nullptr);
  ASSERT_TRUE(reb->right->right == nullptr);
  ASSERT_TRUE(reb->right->balance == 0);
}

TEST(avlTest, test1) {
  // ├── gt:[v:2|b:2]
  // │   └── gt:[v:3|b:1]
  // │       └── gt:[v:4|b:0]

  auto c = new avl::Node<AVLData>(AVLData(2));
  c->balance = 2;
  auto b = c->right = new avl::Node<AVLData>(AVLData(3), c);
  b->balance = 1;
  auto a = b->right = new avl::Node<AVLData>(AVLData(4), b);
  a->balance = 0;
  auto reb = avl::impl::avl::rotate_left(c);

  ASSERT_TRUE(reb->value == 3);
  ASSERT_TRUE(reb->parent == nullptr);
  ASSERT_TRUE(reb->balance == 0);

  ASSERT_TRUE(reb->left->value == 2);
  ASSERT_TRUE(reb->left->parent);
  ASSERT_TRUE(reb->left->parent == b);
  ASSERT_TRUE(reb->left->left == nullptr);
  ASSERT_TRUE(reb->left->right == nullptr);
  ASSERT_TRUE(reb->left->balance == 0);

  ASSERT_TRUE(reb->right->value == 4);
  ASSERT_TRUE(reb->right->parent);
  ASSERT_TRUE(reb->right->parent == b);
  ASSERT_TRUE(reb->right->left == nullptr);
  ASSERT_TRUE(reb->right->right == nullptr);
  ASSERT_TRUE(reb->right->balance == 0);
}

TEST(avlTest, test_increasing_order) 
{
  avl::Tree<AVLData> tree;
  int i = 0;
  for (; i < 10; ++i) {
    printf(".%d <- ", i);
    AVLData ins(i);
    auto res = avl::insert(tree, ins);
    ASSERT_TRUE(std::get<1>(res));
    ASSERT_TRUE(std::get<0>(res) != nullptr);
    if (!avl::verify(tree)) {
      dump(tree);
      ASSERT_TRUE(false);
    }
    printf("\n--------------\n");
  }
}

TEST(avlTest, test_1a) 
{
  avl::Tree<int> tree;
  {
    avl::insert(tree, 20);
    auto root = tree.root;
    ASSERT_TRUE(root);
    ASSERT_TRUE(root->value == 20);
    ASSERT_TRUE(root->balance == 0);

    ASSERT_TRUE(root->right == nullptr);
    ASSERT_TRUE(root->left == nullptr);
  }

  {
    avl::insert(tree, 4);
    auto root = tree.root;
    ASSERT_TRUE(root);
    ASSERT_TRUE(root->value == 20);
    ASSERT_TRUE(root->balance == -1);

    ASSERT_TRUE(root->right == nullptr);

    ASSERT_TRUE(root->left);
    ASSERT_TRUE(root->left->value == 4);
    ASSERT_TRUE(root->left->balance == 0);
    ASSERT_TRUE(root->left->left == nullptr);
    ASSERT_TRUE(root->left->right == nullptr);
  }

  {
    avl::insert(tree, 15);
    auto root = tree.root;
    ASSERT_TRUE(root);
    ASSERT_TRUE(root->value == 15);
    ASSERT_TRUE(root->balance == 0);

    ASSERT_TRUE(root->right);
    ASSERT_TRUE(root->right->value == 20);
    ASSERT_TRUE(root->right->balance == 0);
    ASSERT_TRUE(root->right->left == nullptr);
    ASSERT_TRUE(root->right->right == nullptr);

    ASSERT_TRUE(root->left);
    ASSERT_TRUE(root->left->value == 4);
    ASSERT_TRUE(root->left->balance == 0);
    ASSERT_TRUE(root->left->left == nullptr);
    ASSERT_TRUE(root->left->right == nullptr);
  }
}

TEST(avlTest, test_1b) {
  avl::Tree<int> tree;
  avl::insert(tree, 20);
  {
    avl::insert(tree, 8);
    auto root = tree.root;
    ASSERT_TRUE(root);
    ASSERT_TRUE(root->value == 20);
    ASSERT_TRUE(root->balance == -1);

    ASSERT_TRUE(root->right == nullptr);

    ASSERT_TRUE(root->left);
    ASSERT_TRUE(root->left->value == 8);
    ASSERT_TRUE(root->left->balance == 0);
    ASSERT_TRUE(root->left->left == nullptr);
    ASSERT_TRUE(root->left->right == nullptr);
  }
  {
    avl::insert(tree, 4);
    auto root = tree.root;
    ASSERT_TRUE(root);
    ASSERT_TRUE(root->value == 8);
    ASSERT_TRUE(root->balance == 0);

    ASSERT_TRUE(root->right);
    ASSERT_TRUE(root->right->value == 20);
    ASSERT_TRUE(root->right->balance == 0);
    ASSERT_TRUE(root->right->left == nullptr);
    ASSERT_TRUE(root->right->right == nullptr);

    ASSERT_TRUE(root->left);
    ASSERT_TRUE(root->left->value == 4);
    ASSERT_TRUE(root->left->balance == 0);
    ASSERT_TRUE(root->left->left == nullptr);
    ASSERT_TRUE(root->left->right == nullptr);
  }
}

#if 0
// TODO
TEST(avlTest, test_inc_insert_dec_remove)
{
  avl::Tree<int> tree;
  int i = 0;
  for (; i < 10; ++i) {
    printf(".%d <- \n", i);
    auto res = avl::insert(tree, i);
    ASSERT_TRUE(std::get<1>(res));
    ASSERT_TRUE(std::get<0>(res) != nullptr);
    dump(tree);
    avl::verify(tree);
    printf("\n--------------\n");
  }
  --i;
  for (; i > 0; --i) {
    printf(".%d -> \n", i);
    auto res = avl::remove(tree, i);
    printf("%d = av::remove(tree, %d)\n", res, i);
    ASSERT_TRUE(res);
    dump(tree);
    avl::verify(tree);
    printf("\n--------------\n");
  }
}

TEST(avlTest, test_remove_all)
{
  for (int i = 0; i < 20; ++i) {
    printf("##remove all[%d]\n", i);
    avl::Tree<int> tree;
    for (int d = 0; d <= i; ++d) {
      auto res = avl::insert(tree, d);
      /*assert*/ {
        int *const iptr = std::get<0>(res);
        ASSERT_TRUE(std::get<1>(res) == true);
        ASSERT_TRUE(iptr);
        ASSERT_TRUE(*iptr == d);
        if (!avl::verify(tree)) {
          avl::dump(tree, "\033[92minsert\033[0m|");
          ASSERT_TRUE(false);
        }
      }
    }
    avl::dump(tree, "\033[92mbefore\033[0m|");

    for (int d = 0; d <= i; ++d) {
      auto res = avl::remove(tree, d);
      /*assert*/ {
        ASSERT_TRUE(res);
        if (!avl::verify(tree)) {
          avl::dump(tree, "\033[91mremove\033[0m|");
          ASSERT_TRUE(false);
        }
const int* fr = avl::find(tree, d)
        ASSERT_EQ(fr , nullptr);
      }
    }
    ASSERT_EQ(tree.root , nullptr);
    printf("------\n\n");
  }
}
#endif
