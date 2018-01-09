#include <tree/avl.h>
#include <tree/avl_insert.h>
#include <tree/avl_remove.h>

#include "gtest/gtest.h"

#include <random>

struct Data {
  using T = int;
  bool present;
  T data;
  Data()
      : present(false)
      , data(0) {
  }
  explicit Data(T d)
      : present(true)
      , data(d) {
  }

  explicit operator bool() const noexcept {
    return present;
  }

  Data &
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
  operator==(const Data &o) const noexcept {
    return operator==(o.data);
  }

  bool
  operator>(const T &o) const noexcept {
    return data > o;
  }

  bool
  operator>(const Data &o) const noexcept {
    return operator>(o.data);
  }

  bool
  operator<(const T &o) const noexcept {
    return data < o;
  }

  bool
  operator<(const Data &o) const noexcept {
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
  cmp(const Data &o) const noexcept {
    return cmp(o.data);
  }
};

TEST(avlTest, rotate_left) {
  auto a = new avl::Node<Data>(Data(1));
  a->balance = 2;
  auto b = a->right = new avl::Node<Data>(Data(2), a);
  b->balance = 1;
  auto c = b->right = new avl::Node<Data>(Data(3), b);
  c->balance = 0;

  auto reb = avl::impl::avl::rotate_left(a);
  assert(reb->value == b->value);
  assert(reb->parent == nullptr);
  assert(reb->balance == 0);

  assert(reb->left->value == a->value);
  assert(reb->left->parent == b);
  assert(reb->left->left == nullptr);
  assert(reb->left->right == nullptr);
  assert(reb->left->balance == 0);

  assert(reb->right->value == c->value);
  assert(reb->right->parent == b);
  assert(reb->right->left == nullptr);
  assert(reb->right->right == nullptr);
  assert(reb->right->balance == 0);
}

TEST(avlTest, rotate_right) {
  auto c = new avl::Node<Data>(Data(3));
  c->balance = -2;
  auto b = c->left = new avl::Node<Data>(Data(2), c);
  b->balance = -1;
  auto a = b->left = new avl::Node<Data>(Data(1), b);
  a->balance = 0;

  auto reb = avl::impl::avl::rotate_right(c);
  assert(reb->value == b->value);
  assert(reb->parent == nullptr);
  assert(reb->balance == 0);

  assert(reb->left->value == a->value);
  assert(reb->left->parent == b);
  assert(reb->left->left == nullptr);
  assert(reb->left->right == nullptr);
  assert(reb->left->balance == 0);

  assert(reb->right->value == c->value);
  assert(reb->right->parent == b);
  assert(reb->right->left == nullptr);
  assert(reb->right->right == nullptr);
  assert(reb->right->balance == 0);
}

TEST(avlTest, double_rotate_left) {
  auto c = new avl::Node<Data>(Data(3));
  c->balance = -2;
  auto b = c->left = new avl::Node<Data>(Data(1), c);
  b->balance = 1;
  auto a = b->right = new avl::Node<Data>(Data(2), b);
  a->balance = 0;

  c->left = avl::impl::avl::rotate_left(c->left);
  auto reb = avl::impl::avl::rotate_right(c);
  assert(reb->value == 2);
  assert(reb->parent == nullptr);
  assert(reb->balance == 0);

  assert(reb->left->value == 1);
  assert(reb->left->parent);
  assert(reb->left->parent == a);
  assert(reb->left->left == nullptr);
  assert(reb->left->right == nullptr);
  assert(reb->left->balance == 0);

  assert(reb->right->value == 3);
  assert(reb->right->parent);
  assert(reb->right->parent == a);
  assert(reb->right->left == nullptr);
  assert(reb->right->right == nullptr);
  assert(reb->right->balance == 0);
}

TEST(avlTest, double_rotate_right) {
  auto c = new avl::Node<Data>(Data(1));
  c->balance = 2;
  auto b = c->right = new avl::Node<Data>(Data(3), c);
  b->balance = -1;
  auto a = b->left = new avl::Node<Data>(Data(2), b);
  a->balance = 0;

  c->right = avl::impl::avl::rotate_right(c->right);
  auto reb = avl::impl::avl::rotate_left(c);
  assert(reb->value == 2);
  assert(reb->parent == nullptr);
  assert(reb->balance == 0);

  assert(reb->left->value == 1);
  assert(reb->left->parent);
  assert(reb->left->parent == a);
  assert(reb->left->left == nullptr);
  assert(reb->left->right == nullptr);
  assert(reb->left->balance == 0);

  assert(reb->right->value == 3);
  assert(reb->right->parent);
  assert(reb->right->parent == a);
  assert(reb->right->left == nullptr);
  assert(reb->right->right == nullptr);
  assert(reb->right->balance == 0);
}

TEST(avlTest, test1) {
  // ├── gt:[v:2|b:2]
  // │   └── gt:[v:3|b:1]
  // │       └── gt:[v:4|b:0]

  auto c = new avl::Node<Data>(Data(2));
  c->balance = 2;
  auto b = c->right = new avl::Node<Data>(Data(3), c);
  b->balance = 1;
  auto a = b->right = new avl::Node<Data>(Data(4), b);
  a->balance = 0;
  auto reb = avl::impl::avl::rotate_left(c);

  assert(reb->value == 3);
  assert(reb->parent == nullptr);
  assert(reb->balance == 0);

  assert(reb->left->value == 2);
  assert(reb->left->parent);
  assert(reb->left->parent == b);
  assert(reb->left->left == nullptr);
  assert(reb->left->right == nullptr);
  assert(reb->left->balance == 0);

  assert(reb->right->value == 4);
  assert(reb->right->parent);
  assert(reb->right->parent == b);
  assert(reb->right->left == nullptr);
  assert(reb->right->right == nullptr);
  assert(reb->right->balance == 0);
}

TEST(avlTest, test_increasing_order) 
{
  avl::Tree<Data> tree;
  int i = 0;
  for (; i < 10; ++i) {
    printf(".%d <- ", i);
    Data ins(i);
    auto res = avl::insert(tree, ins);
    assert(std::get<1>(res));
    assert(std::get<0>(res) != nullptr);
    if (!avl::verify(tree)) {
      dump(tree);
      assert(false);
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
    assert(root);
    assert(root->value == 20);
    assert(root->balance == 0);

    assert(root->right == nullptr);
    assert(root->left == nullptr);
  }

  {
    avl::insert(tree, 4);
    auto root = tree.root;
    assert(root);
    assert(root->value == 20);
    assert(root->balance == -1);

    assert(root->right == nullptr);

    assert(root->left);
    assert(root->left->value == 4);
    assert(root->left->balance == 0);
    assert(root->left->left == nullptr);
    assert(root->left->right == nullptr);
  }

  {
    avl::insert(tree, 15);
    auto root = tree.root;
    assert(root);
    assert(root->value == 15);
    assert(root->balance == 0);

    assert(root->right);
    assert(root->right->value == 20);
    assert(root->right->balance == 0);
    assert(root->right->left == nullptr);
    assert(root->right->right == nullptr);

    assert(root->left);
    assert(root->left->value == 4);
    assert(root->left->balance == 0);
    assert(root->left->left == nullptr);
    assert(root->left->right == nullptr);
  }
}

TEST(avlTest, test_1b) {
  avl::Tree<int> tree;
  avl::insert(tree, 20);
  {
    avl::insert(tree, 8);
    auto root = tree.root;
    assert(root);
    assert(root->value == 20);
    assert(root->balance == -1);

    assert(root->right == nullptr);

    assert(root->left);
    assert(root->left->value == 8);
    assert(root->left->balance == 0);
    assert(root->left->left == nullptr);
    assert(root->left->right == nullptr);
  }
  {
    avl::insert(tree, 4);
    auto root = tree.root;
    assert(root);
    assert(root->value == 8);
    assert(root->balance == 0);

    assert(root->right);
    assert(root->right->value == 20);
    assert(root->right->balance == 0);
    assert(root->right->left == nullptr);
    assert(root->right->right == nullptr);

    assert(root->left);
    assert(root->left->value == 4);
    assert(root->left->balance == 0);
    assert(root->left->left == nullptr);
    assert(root->left->right == nullptr);
  }
}

#define IGNORE
#ifndef IGNORE
// TODO
TEST(avlTest, test_inc_insert_dec_remove)
{
  avl::Tree<int> tree;
  int i = 0;
  for (; i < 10; ++i) {
    printf(".%d <- \n", i);
    auto res = avl::insert(tree, i);
    assert(std::get<1>(res));
    assert(std::get<0>(res) != nullptr);
    dump(tree);
    avl::verify(tree);
    printf("\n--------------\n");
  }
  --i;
  for (; i > 0; --i) {
    printf(".%d -> \n", i);
    auto res = avl::remove(tree, i);
    printf("%d = av::remove(tree, %d)\n", res, i);
    assert(res);
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
        assert(std::get<1>(res) == true);
        assert(iptr);
        assert(*iptr == d);
        if (!avl::verify(tree)) {
          avl::dump(tree, "\033[92minsert\033[0m|");
          assert(false);
        }
      }
    }
    avl::dump(tree, "\033[92mbefore\033[0m|");

    for (int d = 0; d <= i; ++d) {
      auto res = avl::remove(tree, d);
      /*assert*/ {
        assert(res);
        if (!avl::verify(tree)) {
          avl::dump(tree, "\033[91mremove\033[0m|");
          assert(false);
        }
        assert(sp::find(tree, d) == nullptr);
      }
    }
    assert(tree.root == nullptr);
    printf("------\n\n");
  }
}
#endif

TEST(avlTest, test_random)
{
  std::random_device rd;
  std::mt19937 g(rd());
  std::size_t counter = 0;
  while (counter < 10) {
    if (counter % 100 == 0) {
      printf("cnt: %zu\n", counter);
    }
    avl::Tree<int> tree;
    constexpr int in_size = 1000;
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
      // printf(".%d <- ", i);
      auto res = avl::insert(tree, in[i]);
      int *const iptr = std::get<0>(res);
      assert(std::get<1>(res) == true);
      assert(iptr);
      assert(*iptr == in[i]);

      int *const fptr = sp::find(tree, in[i]);
      assert(fptr);
      assert(fptr == iptr);
      assert(*fptr == *iptr);

      if (!avl::verify(tree)) {
        avl::dump(tree, "after|");
        assert(false);
      }
    }

    counter++;
  }
  // TODO insert duplicate
  // TODO delete non existing

  printf("done\n");
}
