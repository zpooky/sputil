#include <tree/static_tree.h>
#include "gtest/gtest.h"

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

TEST(static_treeTest, find) {
  // TODO the size calculations gives level+1 capacity which is wrong
  /*insert & find*/ 
  constexpr std::size_t levels = 10;
  using Type = sp::static_tree<Data, levels>;
  static_assert(Type::levels == levels, "");
  static_assert(Type::capacity == 2047, "");
  {
    Type tree;
    for (int key = 0; key < int(levels) + 1; ++key) {
      for (int i = 0; i < key; ++i) {
        Data *res = sp::find(tree, Data(i));
        ASSERT_TRUE(res);
        ASSERT_TRUE(res->data == i);
      }
      Data d(key);
      ASSERT_TRUE(sp::find(tree, d) == nullptr);
      ASSERT_TRUE(sp::insert(tree, d));
      printf("============\n");
      ASSERT_TRUE(sp::find(tree, d) != nullptr);
    }
    printf("done insert\n");
  }
  {
    Type tree;
    for (int key = levels + 1; key > 0; --key) {
      Data d(key);
      ASSERT_TRUE(sp::find(tree, d) == nullptr);
      ASSERT_TRUE(sp::insert(tree, d));
      ASSERT_TRUE(sp::find(tree, d) != nullptr);
    }
    printf("done reverse insert\n");
  }
}

TEST(static_treeTest, lookup_relative) {
  using namespace sp::impl::static_tree;
  const Direction left = Direction::LEFT;
  const Direction right = Direction::RIGHT;
  {
    sp::relative_idx parent_idx(0);
    for (std::size_t level = 0; level < 63; ++level) {
      {
        sp::relative_idx idx = lookup_relative(parent_idx, left);
        auto a_id = parent_relative(idx);
        ASSERT_TRUE(a_id == parent_idx);
      }
      {
        sp::relative_idx idx = lookup_relative(parent_idx, right);
        // printf("lookup :: parent[%zu] -> %s -> r_idx[%zu]\n", //
        //        std::size_t(parent_idx), "right", std::size_t(idx));

        auto a_id = parent_relative(idx);
        // printf("parent :: level[%zu] -> r_idx[%zu] -> parent[%zu]\n", //
        //        level + 1, std::size_t(idx), std::size_t(a_id));
        ASSERT_TRUE(a_id == parent_idx);
        parent_idx = idx;
      }
    }
  }
  printf("--------\n");
  {
    sp::relative_idx parent_idx(0);
    for (std::size_t level = 0; level < 63; ++level) {
      {
        sp::relative_idx idx = lookup_relative(parent_idx, right);
        auto a_id = parent_relative(idx);
        ASSERT_TRUE(a_id == parent_idx);
      }
      {
        sp::relative_idx idx = lookup_relative(parent_idx, left);
        // printf("lookup :: parent[%zu] -> %s -> r_idx[%zu]\n", //
        //        std::size_t(parent_idx), "left", std::size_t(idx));

        auto a_id = parent_relative(idx);
        // printf("parent :: level[%zu] -> r_idx[%zu] -> parent[%zu]\n", //
        //        level + 1, std::size_t(idx), std::size_t(a_id));
        ASSERT_TRUE(a_id == parent_idx);

        parent_idx = idx;
      }
    }
  }
  printf("--------\n");
}

TEST(static_treeTest, insert_in_order_comare) {
  printf("in_order\n");
  constexpr std::size_t levels = 2;
  using Type = sp::static_tree<Data, levels>;
  {
    Type tree;
    sp::insert(tree, Data(3));

    sp::insert(tree, Data(1));
    sp::insert(tree, Data(2));
    sp::insert(tree, Data(0));

    sp::insert(tree, Data(5));
    sp::insert(tree, Data(4));
    sp::insert(tree, Data(6));

    printf("insert: ");
    for (std::size_t i = 0; i < Type::capacity; ++i) {
      printf("%d,", tree.storagex[i].data);
    }
    printf("\n");
    for (std::size_t i = 0; i < Type::capacity; ++i) {
      ASSERT_TRUE(sp::find(tree, i));
    }
  }
  {
    Type tree;
    int i = 0;
    sp::in_order_for_each(tree, [&i](Data &d) {
        ASSERT_TRUE(!bool(d));
        printf("in_order(%d)", i);
        printf("in_order(%d)", d.data);
        d = Data(i);
        printf("in_order(%d)\n", d.data);
        ASSERT_TRUE(bool(d));
        ++i;
        });
    // printf("inserted[%d],capacity[%zu]\n", i, Type::capacity);
    ASSERT_TRUE(i == Type::capacity);

    printf("in_order: ");
    for (std::size_t i = 0; i < Type::capacity; ++i) {
      printf("%d,", tree.storagex[i].data);
    }
    printf("\n");
    for (std::size_t i = 0; i < Type::capacity; ++i) {
      ASSERT_TRUE(sp::find(tree, i));
    }
  }
  printf("--------\n");
}

TEST(static_treeTest, in_order_for_each) {
  constexpr std::size_t levels = 9;
  using Type = sp::static_tree<Data, levels>;
  static_assert(Type::levels == levels, "");
  // static_assert(Type::capacity == 1023, "");
  Type tree;
  for (std::size_t i = 0; i < Type::capacity; ++i) {
    Data d((int)i);
    ASSERT_TRUE(sp::find(tree, d) == nullptr);
  }
  {
    int i = 0;
    sp::in_order_for_each(tree, [&i](Data &d) {
        ASSERT_TRUE(!bool(d));
        d = Data(i);
        ++i;
        });
    printf("inserted[%d],capacity[%zu]\n", i, Type::capacity);
    ASSERT_TRUE(i == Type::capacity);
  }
  {
    int i = 0;
    sp::in_order_for_each(tree, [&i](Data &d) {
        ASSERT_TRUE(bool(d));
        ASSERT_TRUE(d.data == i);
        ++i;
        });
    printf("present[%d],capacity[%zu]\n", i, Type::capacity);
    ASSERT_TRUE(i == Type::capacity);
  }
  for (std::size_t i = 0; i < Type::capacity; ++i) {
    Data d((int)i);
    ASSERT_TRUE(sp::find(tree, d) != nullptr);
  }
  printf("find ok!\n");
}

TEST(static_treeTest, junk) {
  constexpr std::size_t levels = 9;
  using Type = sp::static_tree<Data, levels>;
  Type tree;
  Data d(1);
  insert(tree, d);
  {
    constexpr std::size_t levels = 2;
    using Type = sp::static_tree<Data, levels>;
    Type tree;
    int i = 0;
    sp::in_order_for_each(tree, [&i](auto &node) {
        node = i++;
        //
        });
    {
      int cmp = 0;
      sp::in_order_for_each(tree, [&cmp](auto node) {
          ASSERT_TRUE(node == cmp);
          cmp++;
          });
      ASSERT_TRUE(cmp == i);
      ASSERT_TRUE(cmp == Type::capacity);
      printf("cmp: %d\n", cmp);
    }
    {
      printf("in_order: ");
      for (std::size_t i = 0; i < Type::capacity; ++i) {
        printf("%d,", tree.storagex[i].data);
      }
      printf("\n");
    }
    for (int k = 0; k < int(Type::capacity); ++k) {
      auto *r = sp::find(tree, k);
      printf("k[%d] < i[%d]\n", k, i);
      ASSERT_TRUE(r);
      if (r) {
        ASSERT_TRUE(*r == k);
      }
    }
  }
}
