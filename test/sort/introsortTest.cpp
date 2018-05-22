#include <collection/Array.h>
#include <gtest/gtest.h>
#include <prng/xorshift.h>
#include <sort/introsort.h>
#include <sort/util.h>

template <typename T>
static void
print_arr(T &arr, std::size_t length, std::size_t idx = ~std::size_t(0)) {
  for (std::size_t i = 0; i < length; ++i) {
    if (i == idx) {
      printf("\033[92m%d\033[0m, ", arr[i]);
    } else {
      printf("%d, ", arr[i]);
    }
  }
  printf("\n");
}

template <typename T, typename Comparator>
static void
check(T *arr, std::size_t length) {
  // printf("------\n");
  // print_arr(arr, length);
  sp::rec::introsort<T, Comparator>(arr, length);

  // print_arr(arr, length);

  bool result = sp::is_sorted<T, Comparator>(arr, length);
  ASSERT_TRUE(result);

  // printf("--\n");
}

TEST(IntrosortTest, test_greater) {
  prng::xorshift32 r(1);
  sp::StaticArray<int, 1024> arr;
  for (int i = 0; i < int(capacity(arr)); ++i) {
    ASSERT_TRUE(insert(arr, i));
  }
  ASSERT_EQ(capacity(arr), length(arr));

  for (std::size_t a = 0; a < 1024; ++a) {
    shuffle(r, arr);

    check<int, sp::greater>(arr.raw, length(arr));
    ASSERT_EQ(capacity(arr) - 1, arr[0]);
  }
}

TEST(IntrosortTest, test_lesser) {
  prng::xorshift32 r(1);
  sp::StaticArray<int, 1024> arr;
  for (int i = 0; i < int(capacity(arr)); ++i) {
    ASSERT_TRUE(insert(arr, i));
  }
  ASSERT_EQ(capacity(arr), length(arr));

  for (std::size_t a = 0; a < 1024; ++a) {
    shuffle(r, arr);

    check<int, sp::less>(arr.raw, length(arr));
    ASSERT_EQ(0, arr[0]);
  }
}
