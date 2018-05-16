#include <gtest/gtest.h>
#include <util/permutation.h>

TEST(permutationTest, perm_test2_2) {
  std::size_t res = 0;
  ASSERT_TRUE(sp::rec::permutations("01", 2, [&res](const auto &str) {
    /**/
    printf("%s\n", str);
    ++res;
    return true;
  }));
  ASSERT_EQ(std::size_t(4), res);
}

TEST(permutationTest, perm_test2_3) {
  std::size_t res = 0;
  ASSERT_TRUE(sp::rec::permutations("01", 2, 3, [&res](const auto &str) {
    /**/
    printf("%s\n", str);
    ++res;
    return true;
  }));
  ASSERT_EQ(std::size_t(8), res);
}

TEST(permutationTest, perm_test3_3) {
  std::size_t res = 0;
  ASSERT_TRUE(sp::rec::permutations("012", 3, 3, [&res](const auto &str) {
    /**/
    printf("%s\n", str);
    ++res;
    return true;
  }));
  ASSERT_EQ(std::size_t(27), res);
}

TEST(permutationTest, combination_test) {
  std::size_t res = 0;
  const char *in = "01234";
  ASSERT_TRUE(sp::rec::combinations(in, strlen(in), [&res](const auto &str) {
    // for_each(str, [](const char *c) {
    //   #<{(||)}>#
    //   printf("%c", *c);
    // });

    // printf("\n");
    ++res;
    return true;
  }));
  ASSERT_EQ(std::size_t(120), res);
  ASSERT_EQ(sp::rec::combinations_of(strlen(in)), res);
}

TEST(permutationTest, optionals_rwx) {
  std::size_t res = 0;
  const char *in = "rwx";
  ASSERT_TRUE(sp::rec::optionals(in, strlen(in), [&res](const auto &str) {
    for_each(str, [](const char *c) {
      /**/
      printf("%c", *c);
    });

    printf("\n");

    ++res;
    return true;
  }));
  ASSERT_EQ(std::size_t(8), res);
}

TEST(permutationTest, rwx) {
  sp::UinStaticArray<std::string, 128> access;

  const char *in = "rwx";
  ASSERT_TRUE(sp::rec::optionals(in, strlen(in), [&access](const auto &str) {
    std::string msg;
    for_each(str, [&msg](const char *c) {
      /**/
      msg.push_back(*c);
    });

    insert(access, std::move(msg));
    return true;
  }));

  std::size_t combs = 0;
  std::size_t res_length = 3;
  sp::rec::permutations(access.data(), length(access), res_length,
                        [&combs](const auto &str) {
                          (void)str;
                          // const char prefix[] = {'u', 'g', 'o'};
                          // const char *pit = prefix;
                          //
                          // for_each(str, [&pit, &prefix](const std::string *c)
                          // {
                          //   #<{(||)}>#
                          //   if (pit != prefix) {
                          //     printf(",");
                          //   }
                          //   printf("%c=", *pit++);
                          //   printf("%s", c->c_str());
                          // });
                          // printf("\n");
                          ++combs;

                          return true;
                        });
  // printf("%zu combinations\n", combs);
  ASSERT_EQ(combs, sp::rec::permutations_of(length(access), res_length));
  // ASSERT_EQ(std::size_t(2), res);
}
