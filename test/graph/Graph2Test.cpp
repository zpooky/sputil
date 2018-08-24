#include <collection/Array.h>
#include <graph/graph2.h>
#include <gtest/gtest.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <util/Bitset.h>

TEST(Graph2Test, test) {
  graph::Vertex<int> one(1);
  graph::Vertex<int> two(2);
  ASSERT_FALSE(is_adjacent(one, one));
  ASSERT_FALSE(is_adjacent(one, two));
  ASSERT_FALSE(is_adjacent(two, one));

  ASSERT_TRUE(add_edge(one, 0, &two));
  ASSERT_TRUE(is_adjacent(one, two));
  ASSERT_FALSE(is_adjacent(two, one));

  ASSERT_FALSE(add_edge(one, 0, &two));
  ASSERT_TRUE(is_adjacent(one, two));
  ASSERT_FALSE(is_adjacent(two, one));

  ASSERT_TRUE(add_edge(two, 0, &one));
  ASSERT_TRUE(is_adjacent(one, two));
  ASSERT_TRUE(is_adjacent(two, one));

  ASSERT_FALSE(add_edge(two, 0, &one));
  ASSERT_TRUE(is_adjacent(one, two));
  ASSERT_TRUE(is_adjacent(two, one));
}

TEST(Graph2Test, add_edge) {
  constexpr std::size_t cap = 100;
  sp::UinStaticArray<graph::Vertex<std::size_t>, cap> arr;
  for (std::size_t i = 0; i < cap; ++i) {
    auto res = insert(arr, i);
    ASSERT_TRUE(res);
  }

  for (std::size_t i = 0; i < length(arr); ++i) {
    graph::Vertex<std::size_t> *current = get(arr, i);
    ASSERT_TRUE(current);
    ASSERT_FALSE(is_full(current->edges));

    //==============
    for (std::size_t k = 0; k < length(arr); ++k) {
      graph::Vertex<std::size_t> *other = get(arr, k);
      if (i != k) {
        ASSERT_TRUE(other);
        ASSERT_FALSE(is_adjacent(*current, *other));
        ASSERT_TRUE(add_edge(*current, 0, other));
        ASSERT_TRUE(is_adjacent(*current, *other));
      }
    }
  }
  printf("asd\n");
}

TEST(Graph2Test, dfs) {
  printf("--\n");
  constexpr std::size_t cap = 256;
  printf("--\n");
  sp::UinStaticArray<graph::Vertex<std::size_t>, cap> arr;
  printf("--\n");
  for (std::size_t i = 0; i < cap; ++i) {
    auto res = insert(arr, i);
    ASSERT_TRUE(res);
  }
  printf("--\n");
  printf("size: %zubytes, %zukB, %zuMB\n", //
         sizeof(arr), sizeof(arr) / 1024, sizeof(arr) / 1024 / 1024);

  printf("--\n");
  ASSERT_TRUE(is_full(arr));
  prng::xorshift32 r(1);
  //===
  for (std::size_t i = 0; i < length(arr); ++i) {
    graph::Vertex<std::size_t> *current = get(arr, i);
    ASSERT_TRUE(current);
    const auto edges = uniform_dist(r, 0, 128);

    for (std::size_t k = 0; k < edges; ++k) {
      std::size_t idx = uniform_dist(r, 0, length(arr));

      if (idx != i) {
        graph::Vertex<std::size_t> *other = get(arr, idx);
        ASSERT_TRUE(other);
        ASSERT_FALSE(is_full(other->edges));
        ASSERT_FALSE(is_full(current->edges));

        if (!is_adjacent(*current, *other)) {
          ASSERT_FALSE(is_adjacent(*current, *other));
          ASSERT_FALSE(is_adjacent(*other, *current));

          ASSERT_TRUE(add_edge(*current, 0, other));
          ASSERT_TRUE(is_adjacent(*current, *other));

          ASSERT_FALSE(is_adjacent(*other, *current));
          ASSERT_TRUE(add_edge(*other, 0, current));
          ASSERT_TRUE(is_adjacent(*other, *current));
        }
      }
    }
  }
  //===
  {
    sp::StaticBitset<cap> visited;
    graph::Vertex<std::size_t> &root = arr[0];
    std::size_t cnt = 0;
    bool res = deapth_first(root, [&cnt, &visited](const auto &current) {
      // printf("%zu\n", current.value);
      cnt++;
      ASSERT_FALSE(set(visited, current.value, true));
    });
    ASSERT_TRUE(res);
    printf("cnt: %zu\n", cnt);
    ASSERT_EQ(cap, cnt);
    for_each(visited, [cap](std::size_t idx, bool v) {
      if (idx < cap) {
        ASSERT_TRUE(v);
      } else {
        ASSERT_FALSE(v);
      }
    });
  }
  //===
  {
    sp::StaticBitset<cap> visited;
    graph::Vertex<std::size_t> &root = arr[0];
    std::size_t cnt = 0;
    bool res = breadth_first(root, [&cnt, &visited](const auto &current) {
      // printf("%zu\n", current.value);
      cnt++;
      ASSERT_FALSE(set(visited, current.value, true));
    });
    ASSERT_TRUE(res);
    printf("cnt: %zu\n", cnt);
    ASSERT_EQ(cap, cnt);
    for_each(visited, [cap](std::size_t idx, bool v) {
      if (idx < cap) {
        ASSERT_TRUE(v);
      } else {
        ASSERT_FALSE(v);
      }
    });
  }
}
