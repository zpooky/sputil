#include "gtest/gtest.h"
#include <util/maybe.h>

TEST(MaybeTest, test_map) {
  sp::maybe<int> m{1};
  ASSERT_TRUE(bool(m));
  ASSERT_EQ(m.get(),1);
  int cap =0;
  sp::maybe<char> res = sp::map(m, [&cap](int&in) -> char {
      cap = in;
      return 'c';
      });
  
  ASSERT_TRUE(bool(m));
  ASSERT_EQ(m.get(),1);
  ASSERT_EQ(cap,1);
  ASSERT_TRUE(bool(res));
  ASSERT_EQ(res.get(),'c');
  ASSERT_EQ(res.get_or('a'),'c');
}

TEST(MaybeTest, test_const_map) {
  const sp::maybe<int> m{1};
  ASSERT_TRUE(bool(m));
  int cap =0;
  sp::maybe<char> res = sp::map(m, [&cap](const int&in) -> char {
      cap = in;
      return 'c';
      });

  ASSERT_TRUE(bool(m));
  ASSERT_EQ(m.get(),1);

  ASSERT_EQ(cap,1);
  ASSERT_TRUE(bool(res));
  ASSERT_EQ(res.get(),'c');
  ASSERT_EQ(res.get_or('a'),'c');
}

TEST(MaybeTest, test_forward_map) {
  int cap =0;
  sp::maybe<char> res = sp::map(sp::maybe<int>{1}, [&cap](int&&in) -> char {
      cap = in;
      return 'c';
      });

  ASSERT_EQ(cap,1);

  ASSERT_TRUE(bool(res));
  ASSERT_EQ(res.get(),'c');
  ASSERT_EQ(res.get_or('a'),'c');
}

TEST(MaybeTest, test_map_none) {
  sp::maybe<int> m{};
  ASSERT_FALSE(bool(m));
  bool cap=false;
  sp::maybe<char> res = sp::map(m, [&cap](int&) -> char {
      cap=true;
      return 'c';
      });

  ASSERT_FALSE(bool(m));

  ASSERT_FALSE(cap);
  ASSERT_FALSE(bool(res));
  ASSERT_EQ(res.get_or('a'),'a');
}
