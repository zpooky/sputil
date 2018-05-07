#include <graph/Graph.h>
#include <gtest/gtest.h>
#include <prng/util.h>
#include <prng/xorshift.h>

TEST(graphTest, test) {
  graph::Undirected<int> first(1);
  auto *second = add_vertex(first, 2);
  {
    ASSERT_TRUE(second);
    ASSERT_EQ(second->value, 2);
    ASSERT_TRUE(is_adjacent(first, *second));
    ASSERT_TRUE(is_adjacent(*second, first));
  }
  graph::Undirected<int> third(3);
  {
    ASSERT_FALSE(is_adjacent(first, third));
    ASSERT_FALSE(is_adjacent(*second, third));

    ASSERT_TRUE(add_edge(first, &third));
    ASSERT_TRUE(is_adjacent(first, third));
    ASSERT_TRUE(is_adjacent(third, first));
    ASSERT_FALSE(is_adjacent(third, *second));
  }
  {
    graph::Undirected<int> dummy(0);
    ASSERT_FALSE(is_adjacent(first, dummy));
    ASSERT_FALSE(is_adjacent(*second, dummy));
    ASSERT_FALSE(is_adjacent(third, dummy));

    ASSERT_FALSE(remove_edge(first, &dummy));
    ASSERT_FALSE(remove_edge(*second, &dummy));
    ASSERT_FALSE(remove_edge(third, &dummy));
  }
  printf("--\n");
  {
    ASSERT_TRUE(is_adjacent(*second, first));
    ASSERT_TRUE(is_adjacent(first, *second));

    ASSERT_TRUE(remove_edge(first, second));

    ASSERT_FALSE(is_adjacent(*second, first));
    ASSERT_FALSE(is_adjacent(first, *second));

    ASSERT_FALSE(remove_edge(first, second));
    ASSERT_FALSE(remove_edge(*second, &first));
  }
  printf("--\n");
  {
    ASSERT_TRUE(is_adjacent(third, first));
    ASSERT_TRUE(is_adjacent(first, third));

    ASSERT_TRUE(remove_edge(third, &first));

    ASSERT_FALSE(is_adjacent(third, first));
    ASSERT_FALSE(is_adjacent(first, third));

    ASSERT_FALSE(remove_edge(third, &first));
    ASSERT_FALSE(remove_edge(first, &third));
  }
  printf("--\n");
}

TEST(graphTest, test_dtor) {
  // not workign gc
  using g_type = graph::Undirected<int, 26>;
  g_type root(1);
  sp::UinStaticArray<g_type *, g_type::capacity> arr;
  {
    int i = 2;
    while (!is_full(arr)) {
      g_type *c = add_vertex(root, i);
      ASSERT_TRUE(c);
      ASSERT_EQ(i, c->value);
      ASSERT_TRUE(insert(arr, c));
      ++i;
    }
  }

  prng::xorshift32 r(1);
  for (std::size_t i = 0; i < length(arr); ++i) {
    auto &current = arr[i];
    std::uint32_t n = uniform_dist(r, 0, length(arr));
    for (std::uint32_t k = 0; k < n; ++k) {
      std::uint32_t idx = uniform_dist(r, 0, length(arr));
      // printf("add_edge(arr[idx[%lu]]->value[%d], current)\n", idx,
      //        arr[idx]->value);
      bool already = is_adjacent(*arr[idx], *current);
      if (arr[idx] != current) { // do not try for self assignment
        auto res = add_edge(*arr[idx], current);
        ASSERT_EQ(res, !already); // test for no duplicate edges
      } else {
        ASSERT_FALSE(already);
      }
    }
  }

  depth_first(root, [](auto &c) {
      printf("%d\n",c.value);
    /**/
  });

  // ASSERT_TRUE(add_vertex(root, 1337));
}
