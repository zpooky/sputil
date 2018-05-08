#include <gtest/gtest.h>
// #include <hash/fnv.h>
#include <map/Set.h>
// #include <prng/util.h>
// #include <prng/xorshift.h>

static std::size_t
identity(const std::size_t &in) {
  return in;
}

TEST(HashSetTest, test) {
  sp::HashSet<std::size_t, identity> set;
  {
    std::size_t *res = insert(set, 1);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 1);
  }

  {
    std::size_t *res = insert(set, 1);
    ASSERT_FALSE(res);
  }
}
