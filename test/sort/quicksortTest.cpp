#include <collection/Array.h>
#include <gtest/gtest.h>
#include <sort/quicksort.h>

TEST(QuicksortTest, test) {
  sp::StaticArray<int, 1024> arr;
  for (int i = 0; i < capacity(arr); ++i) {
    ASSERT_TRUE(insert(arr, i));
  }
  sp::rec::quicksort(arr.raw, length(arr));
}
