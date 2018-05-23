#include <collection/Array.h>
#include <gtest/gtest.h>
#include <prng/xorshift.h>
#include <sort/quicksort.h>
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

template <typename T, typename C>
static void
check(T *arr, std::size_t length) {
  // print_arr(arr, length);
  sp::rec::quicksort<T, C>(arr, length);

  // print_arr(arr, length);

  bool res = sp::is_sorted<T, C>(arr, length);
  ASSERT_TRUE(res);
  // printf("--\n");
}

TEST(QuicksortTest, test_less) {
  prng::xorshift32 r(1);
  sp::StaticArray<int, 1024> arr;
  for (int i = 0; i < int(capacity(arr)); ++i) {
    ASSERT_TRUE(insert(arr, i));
  }
  ASSERT_EQ(capacity(arr), length(arr));

  for (std::size_t a = 0; a < 1024 /** 128*/; ++a) {
    shuffle(r, arr);
    // print_arr(arr, length(arr));

    check<int, sp::less>(arr.raw, length(arr));
  }
}

TEST(QuicksortTest, test_greater) {
  prng::xorshift32 r(1);
  sp::StaticArray<int, 1024> arr;
  for (int i = 0; i < int(capacity(arr)); ++i) {
    ASSERT_TRUE(insert(arr, i));
  }
  ASSERT_EQ(capacity(arr), length(arr));

  for (std::size_t a = 0; a < 1024 /** 1024*/; ++a) {
    shuffle(r, arr);

    check<int, sp::greater>(arr.raw, length(arr));
  }
}

TEST(QuicksortTest, test_inorder) {
  constexpr std::size_t length = 10;
  int arr[length] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  check<int, sp::less>(arr, length);
}

TEST(QuicksortTest, test_reverseorder) {
  constexpr std::size_t length = 10;
  int arr[length] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
  check<int, sp::less>(arr, length);
}

TEST(QuicksortTest, test_some_order) {
  constexpr std::size_t length = 10;
  int arr[length] = {10, 8, 6, 4, 2, 1, 3, 5, 7, 9};
  check<int, sp::less>(arr, length);
}

TEST(QuicksortTest, test_some_order2) {
  constexpr std::size_t length = 10;
  int arr[length] = {9, 7, 5, 3, 1, 2, 4, 6, 8, 10};
  check<int, sp::less>(arr, length);
}
