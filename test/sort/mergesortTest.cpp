#include <collection/Array.h>
#include <gtest/gtest.h>
#include <prng/xorshift.h>
#include <sort/mergesort.h>
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

template <typename T>
static void
check(T *arr, std::size_t length) {
  // print_arr(arr, length);
  sp::rec::mergesort(arr, length);

  // print_arr(arr, length);

  ASSERT_TRUE(sp::is_sorted(arr, length));
  // printf("--\n");
}

//~1900ms
TEST(mergesortTest, test) {
  prng::xorshift32 r(1);
  sp::StaticArray<int, 1024> arr;
  for (int i = 0; i < int(capacity(arr)); ++i) {
    ASSERT_TRUE(insert(arr, i));
  }
  ASSERT_EQ(capacity(arr), length(arr));

  for (std::size_t a = 0; a < 1024; ++a) {
    shuffle(r, arr);
    // print_arr(arr, length(arr));

    check(arr.raw, length(arr));
  }
}
