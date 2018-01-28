#include "gtest/gtest.h"
// #include <util/numeric.h>
#include <list/SkipList.h>
#include <random>

// TODO test duplicate insert

// namespace sp {
// template <typename T>
// static void
// swap(T &o, T &t) noexcept {
//   std::swap(o, t);
// }
// } // namespace sp

TEST(SkipListTest, test) {
  constexpr std::size_t levels = 7;
  sp::SkipList<int, levels> list;

  constexpr std::size_t length = 20;
  int in[length];
  for (int i = 0; i < int(length); ++i) {
    in[i] = i;
  }
  std::mt19937 g(0);

  for (std::size_t cnt = 0; cnt < 10; ++cnt) {
    ASSERT_TRUE(is_empty(list));
    std::shuffle(in, in + length, g);
    for (std::size_t i = 0; i < length; ++i) {
      // printf("#%zu\n",i);
      for (std::size_t a = 0; a < i; ++a) {
        // printf("# should be found\n");
        const int *res = find(list, in[a]);
        // printf("find(list,a[%d]): %p\n",in[a],res);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, in[a]);
      }

      {
        int *res = insert(list, in[i]);
        // printf("###insert(list,i[wd]): %p\n",in[i],res);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, in[i]);
        // dump(list);
      }

      // printf("# should not be found\n");
      for (std::size_t a = i + 1; a < length; ++a) {
        const int *res = find(list, in[a]);
        // printf("find(list,a[%d]): %p\n",in[a],res);
        ASSERT_FALSE(res);
        ASSERT_FALSE(remove(list, in[a]));
        {
          const int def = 999999;
          int out = def;
          ASSERT_FALSE(take(list, in[a], out));
          ASSERT_EQ(out, def);
        }
      }
    }

    // printf("#remove\n");
    std::shuffle(in, in + length, g);
    for (std::size_t i = 0; i < length; ++i) {
      for (std::size_t a = 0; a < i; ++a) {
        const int *res = find(list, in[a]);
        // printf("find(list,a[%d]): %p\n",in[a],res);
        ASSERT_FALSE(res);
        ASSERT_FALSE(remove(list, in[a]));
      }

      {
        bool res = remove(list, in[i]);
        // printf("remove(list,%d): %s\n",in[i],res ? "true" : "false");
        ASSERT_TRUE(res);
        // dump(list);
      }

      for (std::size_t a = i + 1; a < length; ++a) {
        // printf("# should be found\n");
        const int *res = find(list, in[a]);
        // printf("find(list,a[%d]): %p\n",in[a],res);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, in[a]);
      }
    }
    ASSERT_TRUE(is_empty(list));
  }
}

TEST(SkipListTest, test_insert_duplicate) {
  constexpr std::size_t levels = 7;
  sp::SkipList<int, levels> list;

  constexpr std::size_t length = 20;
  int in[length];
  for (int i = 0; i < int(length); ++i) {
    in[i] = i;
  }
  std::mt19937 g(0);

  std::shuffle(in, in + length, g);
  for (std::size_t i = 0; i < length; ++i) {
    // printf("###insert(list,i[wd]): %p\n",in[i],res);
    int *res = insert(list, in[i]);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, in[i]);
    for (std::size_t a = i; a < i; ++a) {
      // printf("# should be found\n");
      const int *res = find(list, in[a]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[a]);
    }
  }
  // printf("##\n");
  for (std::size_t i = 0; i < length; ++i) {
    for (std::size_t a = i; a < length; ++a) {
      // printf("# should be found\n");
      const int *res = find(list, in[a]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[a]);
    }
    // dump(list);
    int *res = insert(list, in[i]);
    // printf("###insert(list,i[%d]): \n",in[i]);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, in[i]);
    // dump(list);
    for (std::size_t a = i; a < length; ++a) {
      // printf("# should be found\n");
      const int *res = find(list, in[a]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[a]);
    }
  }
}

TEST(SkipListTest, test_remove) {
  constexpr std::size_t levels = 7;
  sp::SkipList<int, levels> list;

  constexpr std::size_t length = 20;
  int in[length];
  for (int i = 0; i < int(length); ++i) {
    in[i] = i;
  }
  std::mt19937 g(0);

  for (std::size_t cnt = 0; cnt < 10; ++cnt) {
    ASSERT_TRUE(is_empty(list));
    std::shuffle(in, in + length, g);
    for (std::size_t i = 0; i < length; ++i) {
      int *res = insert(list, in[i]);
      // printf("###insert(list,i[wd]): %p\n",in[i],res);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[i]);
      // dump(list);
    }

    std::shuffle(in, in + length, g);
    for (std::size_t i = 0; i < length; ++i) {
      const int def = 999999;
      int out = def;
      ASSERT_TRUE(take(list, in[i], out));
      ASSERT_EQ(out, in[i]);
    }
    ASSERT_TRUE(is_empty(list));

    for (std::size_t i = 0; i < length; ++i) {
      const int *res = find(list, in[i]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_FALSE(res);
    }
    ASSERT_TRUE(is_empty(list));
  }
}

TEST(SkipListTest, test_swap) {
  constexpr std::size_t levels = 7;
  sp::SkipList<int, levels> one;
  ASSERT_TRUE(insert(one, 1));
  sp::SkipList<int, levels> two;
  swap(one, two);
  {
    auto *res = find(two, 1);
    ASSERT_TRUE(res);
    ASSERT_EQ(1, *res);
    ASSERT_TRUE(insert(two, 2));
  }
  {
    auto *res = find(one, 1);
    ASSERT_FALSE(res);
  }
  swap(one, two);
  {
    auto *res = find(two, 1);
    ASSERT_FALSE(res);
    res = find(two, 2);
    ASSERT_FALSE(res);
  }
  {
    auto *res = find(one, 1);
    ASSERT_TRUE(res);
    ASSERT_EQ(1, *res);

    res = find(one, 1);
    ASSERT_TRUE(res);
    ASSERT_EQ(1, *res);

    ASSERT_TRUE(remove(one, 1));
  }
  swap(one, two);
  {
    auto *res = find(two, 2);
    ASSERT_TRUE(res);
    ASSERT_EQ(2, *res);

    res = find(one, 1);
    ASSERT_FALSE(res);
  }
  {
    auto *res = find(one, 1);
    ASSERT_FALSE(res);

    res = find(one, 2);
    ASSERT_FALSE(res);
  }
}

TEST(SkipListTest, test_unique_insert) {
  constexpr std::size_t levels = 7;
  sp::SkipList<int, levels> list;

  constexpr std::size_t length = 20;
  int in[length];
  for (int i = 0; i < int(length); ++i) {
    in[i] = i;
  }
  std::mt19937 g(0);

  for (std::size_t cnt = 0; cnt < 10; ++cnt) {
    ASSERT_TRUE(is_empty(list));
    std::shuffle(in, in + length, g);
    for (std::size_t i = 0; i < length; ++i) {
      // printf("#%zu\n",i);
      for (std::size_t a = 0; a < i; ++a) {
        // printf("# should be found\n");
        const int *res = find(list, in[a]);
        // printf("find(list,a[%d]): %p\n",in[a],res);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, in[a]);
      }

      {
        auto res = insert_unique(list, in[i]);
        int *ptr = std::get<0>(res);
        ASSERT_TRUE(ptr);
        ASSERT_TRUE(std::get<1>(res));
        ASSERT_EQ(*ptr, in[i]);
      }
      {
        auto res = insert_unique(list, in[i]);
        int *ptr = std::get<0>(res);
        ASSERT_TRUE(ptr);
        ASSERT_FALSE(std::get<1>(res));
        ASSERT_EQ(*ptr, in[i]);
      }

      // printf("# should not be found\n");
      for (std::size_t a = i + 1; a < length; ++a) {
        const int *res = find(list, in[a]);
        // printf("find(list,a[%d]): %p\n",in[a],res);
        ASSERT_FALSE(res);
        ASSERT_FALSE(remove(list, in[a]));
        {
          const int def = 999999;
          int out = def;
          ASSERT_FALSE(take(list, in[a], out));
          ASSERT_EQ(out, def);
        }
      }
    }
    //===========================================
    std::shuffle(in, in + length, g);
    for (std::size_t i = 0; i < length; ++i) {
      // printf("#%zu\n",i);
      for (std::size_t a = 0; a < length; ++a) {
        // printf("# should be found\n");
        const int *res = find(list, in[a]);
        // printf("find(list,a[%d]): %p\n",in[a],res);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, in[a]);
      }

      {
        auto res = insert_unique(list, in[i]);
        int *ptr = std::get<0>(res);
        ASSERT_TRUE(ptr);
        ASSERT_FALSE(std::get<1>(res));
        ASSERT_EQ(*ptr, in[i]);
      }
    }
    //===========================================
    std::shuffle(in, in + length, g);
    for (std::size_t i = 0; i < length; ++i) {
      for (std::size_t a = 0; a < i; ++a) {
        const int *res = find(list, in[a]);
        // printf("find(list,a[%d]): %p\n",in[a],res);
        ASSERT_FALSE(res);
        ASSERT_FALSE(remove(list, in[a]));
      }

      {
        bool res = remove(list, in[i]);
        // printf("remove(list,%d): %s\n",in[i],res ? "true" : "false");
        ASSERT_TRUE(res);
        // dump(list);
      }

      for (std::size_t a = i + 1; a < length; ++a) {
        // printf("# should be found\n");
        const int *res = find(list, in[a]);
        // printf("find(list,a[%d]): %p\n",in[a],res);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, in[a]);
      }
    }
    ASSERT_TRUE(is_empty(list));
  }
}

TEST(SkipListTest, test_unique_insert2) {
  constexpr std::size_t levels = 7;
  sp::SkipList<int, levels> list;
  constexpr std::size_t length = 20;
  int in[length];
  for (int i = 0; i < int(length); ++i) {
    in[i] = i;
  }
  std::mt19937 g(0);

  // for(std::size_t cnt=0;cnt<10;++cnt){
  // ASSERT_TRUE(is_empty(list));
  std::shuffle(in, in + length, g);
  // printf("insert_unique()\n");
  for (std::size_t i = 0; i < length; ++i) {
    for (std::size_t a = 0; a < i; ++a) {
      // printf("# should be found\n");
      const int *res = find(list, in[a]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[a]);
    }

    {
      auto res = insert_unique(list, in[i]);
      // printf("###insert_unique(list,i[%d]): %s\n",in[i],
      // std::get<1>(res)?"true":"false"); dump(list);
      int *ptr = std::get<0>(res);
      ASSERT_TRUE(ptr);
      ASSERT_TRUE(std::get<1>(res));
      ASSERT_EQ(*ptr, in[i]);
    }

    // printf("# should not be found\n");
    for (std::size_t a = i + 1; a < length; ++a) {
      const int *res = find(list, in[a]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_FALSE(res);
      ASSERT_FALSE(remove(list, in[a]));
      {
        const int def = 999999;
        int out = def;
        ASSERT_FALSE(take(list, in[a], out));
        ASSERT_EQ(out, def);
      }
    }
  }
  //===============================
  {
    auto res = insert(list, 999);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 999);
  }
  //===U=insert=same=should=be=false=
  // printf("insert_unique_dup()\n");
  std::shuffle(in, in + length, g);
  for (std::size_t i = 0; i < length; ++i) {
    for (std::size_t a = 0; a < length; ++a) {
      const int *res = find(list, in[a]);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[a]);
    }

    {
      auto res = insert_unique(list, in[i]);
      // printf("###insert_unique(list,in[%d]): %s\n",in[i],
      // std::get<1>(res)?"true":"false"); dump(list);
      int *ptr = std::get<0>(res);
      ASSERT_TRUE(ptr);
      ASSERT_EQ(*ptr, in[i]);
      ASSERT_EQ(false, std::get<1>(res));
    }
  }
  //===============================
  {
    auto res = insert_unique(list, 999);
    int *ptr = std::get<0>(res);
    ASSERT_TRUE(ptr);
    ASSERT_EQ(*ptr, 999);
    ASSERT_EQ(false, std::get<1>(res));
  }
  //=Duplicate=Insert=should=work==
  // dump(list);
  // printf("insert()\n");
  std::shuffle(in, in + length, g);
  for (std::size_t i = 0; i < length; ++i) {
    for (std::size_t a = 0; a < length; ++a) {
      const int *res = find(list, in[a]);
      // printf("find(list,in[%d]): %p\n",in[a],res);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[a]);
    }

    {
      int *res = insert(list, in[i]);
      // printf("###insert(list,in[%d]): %s\n",in[i], res !=
      // nullptr?"true":"false");
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[i]);
      // dump(list);
    }
  }
  // dump(list);
  //=U=insert=same=should=be=false=
  std::shuffle(in, in + length, g);
  for (std::size_t i = 0; i < length; ++i) {
    for (std::size_t a = 0; a < length; ++a) {
      const int *res = find(list, in[a]);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[a]);
    }

    {
      auto res = insert_unique(list, in[i]);
      // printf("###insert_unique(list,in[%d]): %s\n",in[i],
      // std::get<1>(res)?"true":"false"); dump(list);
      int *ptr = std::get<0>(res);
      ASSERT_TRUE(ptr);
      ASSERT_EQ(*ptr, in[i]);
      ASSERT_EQ(false, std::get<1>(res));
    }
  }
  // dump(list);
  //=Take=should=be=OK=============
  std::shuffle(in, in + length, g);
  for (std::size_t i = 0; i < length; ++i) {
    for (std::size_t a = 0; a < length; ++a) {
      const int *res = find(list, in[a]);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[a]);
    }

    {
      const int def = 999999;
      int out = def;
      ASSERT_TRUE(take(list, in[i], out));
      ASSERT_EQ(out, in[i]);
    }
  }
  //=Remove=should=be=OK===========
  std::shuffle(in, in + length, g);
  for (std::size_t i = 0; i < length; ++i) {
    for (std::size_t a = 0; a < i; ++a) {
      const int *res = find(list, in[a]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_FALSE(res);
      ASSERT_FALSE(remove(list, in[a]));
    }

    {
      bool res = remove(list, in[i]);
      // printf("remove(list,%d): %s\n",in[i],res ? "true" : "false");
      ASSERT_TRUE(res);
      // dump(list);
    }

    for (std::size_t a = i + 1; a < length; ++a) {
      // printf("# should be found\n");
      const int *res = find(list, in[a]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[a]);
    }
  }
  ASSERT_TRUE(remove(list, 999));
  ASSERT_TRUE(is_empty(list));
  // }
}
