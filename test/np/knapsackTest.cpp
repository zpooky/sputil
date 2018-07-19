#include <gtest/gtest.h>
#include <np/knapsack.h>

TEST(knapsackTest, test) {
  // item i	    1	  2	  3	  4
  // value val	100	20	60	40
  // weight wt	3	  2	  4	  1
  np::item i4{40, 1};

  np::item i3{60, 4};
  i3.next = &i4;

  np::item i2{20, 2};
  i2.next = &i3;

  np::item i1{100, 3};
  i1.next = &i2;

  auto result = np::knapsack(5, &i1);
  printf("%zu\n", result);
}

template <std::size_t N, typename F>
static std::size_t
tt(np::item (&items)[N], std::size_t max, F f) {
  std::size_t cap = sizeof(items) / sizeof(np::item);
  for (std::size_t i = 0; i < cap - 1; ++i) {
    items[i].next = &items[i + 1];
  }

  return f(max, &items[0]);
}

TEST(knapsackTest, test_small1) {
  np::item items[] = {
      {81, 42}, {42, 42}, {56, 68}, {25, 68}, {14, 77},
      {63, 57}, {75, 17}, {41, 19}, {19, 94}, {12, 34},
  };

  ASSERT_EQ(197, tt(items, 78, np::knapsack));
  ASSERT_EQ(197, tt(items, 78, np::dp::knapsack));
}

TEST(knapsackTest, test_small2) {
  np::item items[] = {
      {100, 18}, {76, 51}, {26, 68}, {79, 44}, {64, 46},
      {18, 65},  {1, 66},  {80, 52}, {93, 47}, {66, 3},
  };

  ASSERT_EQ(259, tt(items, 68, np::knapsack));
  ASSERT_EQ(259, tt(items, 68, np::dp::knapsack));
}

TEST(knapsackTest, test_small3) {
  np::item items[] = {
      {18, 95}, {19, 67}, {5, 77},  {26, 21}, {15, 22},
      {64, 74}, {28, 22}, {18, 92}, {60, 7},  {21, 63},
  };
  ASSERT_EQ(129, tt(items, 72, np::knapsack));
  ASSERT_EQ(129, tt(items, 72, np::dp::knapsack));
}

/*   weight of knapsack                             knapsack weight limit:25
 *   0 1 2 3 4 5 6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
 *  .+-+-+-+-+-+-+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-
 * 0|0 0 0 0 0 0 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 *95|0 0 0 0 0 0 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 *67|0 0 0 0 0 0 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 *77|0 0 0 0 0 0 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 *21|0 0 0 0 0 0 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 26 26 26 26 26
 *22|0 0 0 0 0 0 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 26 26 26 26 26
 *74|0 0 0 0 0 0 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 26 26 26 26 26
 *22|0 0 0 0 0 0 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 26 28 28 28 28
 *92|0 0 0 0 0 0 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 26 28 28 28 28
 * 7|0 0 0 0 0 0 0 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60
 *63|0 0 0 0 0 0 0 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60 60
 *^
 * `--Weight
 */
TEST(knapsackTest, test_small) {
  // value,weight
  np::item items[] = {
      {18, 95}, {19, 67}, {5, 77},  {26, 21}, {15, 22},
      {64, 74}, {28, 22}, {18, 92}, {60, 7},  {21, 63},
  };
  ASSERT_EQ(60, tt(items, 25, np::dp::knapsack));
  ASSERT_EQ(60, tt(items, 25, np::knapsack));
}

TEST(knapsackTest, test_cmp) {
  np::item items[] = {
      {18, 95}, {19, 67}, {5, 77},  {26, 21}, {15, 22},
      {64, 74}, {28, 22}, {18, 92}, {60, 7},  {21, 63},
  };
  for (std::size_t i = 1; i < 250; ++i) {
    auto first = tt(items, i, np::knapsack);
    auto second = tt(items, i, np::dp::knapsack);
    ASSERT_EQ(first, second);
    // printf("knapsack[%zu] max value:%zu/%zu\n", i, first,second);
  }
}
