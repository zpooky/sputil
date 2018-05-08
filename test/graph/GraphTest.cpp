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

template <typename T, std::size_t N>
static void
is_all_adjacent(graph::Undirected<T, N> &c) {
  for (std::size_t i = 0; i < length(c.edges); ++i) {
    auto &edge = c.edges[i];
    // printf("- %p\n", edge.ptr);
    ASSERT_TRUE(edge.ptr);

    ASSERT_TRUE(is_adjacent(*edge.ptr, c));
    ASSERT_TRUE(is_adjacent(c, *edge.ptr));
  }

  bool fail = false;
  for (std::size_t i = 0; i < length(c.edges); ++i) {
    auto &iedge = c.edges[i];
    for (std::size_t a = 0; a < length(c.edges); ++a) {
      auto &aedge = c.edges[a];
      if (a != i) {
        if (iedge.ptr == aedge.ptr) {
          printf("i.%zu %p| a.%zu %p\n", i, iedge.ptr, a, aedge.ptr);
          fail = true;
        }
      }
    }
  }

  ASSERT_FALSE(fail);
}

TEST(graphTest, test_dtor) {
  // not workign gc
  using g_type = graph::Undirected<int, 5>;
  sp::UinStaticArray<g_type *, g_type::capacity> arr;
  int a = 0;
  g_type root(a++);
  {
    while (!is_full(arr)) {
      g_type *c = add_vertex(root, a);
      ASSERT_TRUE(c);
      ASSERT_EQ(a, c->value);
      ASSERT_TRUE(insert(arr, c));
      ++a;
    }
  }
  for (std::size_t i = 0; i < length(arr); ++i) {
    // auto &current = arr[i];
    // printf("current[%d].length[%zu]\n", current->value,
    // length(current->edges));
  }
  printf("--\n");

  prng::xorshift32 r(1);
  for (std::size_t i = 0; i < length(arr); ++i) {
    auto &current = arr[i];
    std::uint32_t n = uniform_dist(r, 0, length(arr));
    for (std::uint32_t k = 0; k < n; ++k) {
      // is_all_adjacent(*current);
      std::uint32_t idx = uniform_dist(r, 0, length(arr));

      printf("add_edge(current[%d], arr[idx[%u]]->value[%d])", arr[i]->value,
             idx, arr[idx]->value);
      const bool already = is_adjacent(*arr[idx], *current);

      if (arr[idx] != current) { // do not try for self assignment
        // is_all_adjacent(*current);
        const auto res = add_edge(*current, arr[idx]);

        printf(" = %s", res ? "true" : "false");
        printf(" edges.length[%zu]\n", length(current->edges));
        is_all_adjacent(*current);
        ASSERT_EQ(res, !already); // test for no duplicate edges
      } else {
        printf(" = self\n");
        ASSERT_FALSE(already);
      }
    }
    printf("--\n");
  }

  depth_first(root, [](auto &c) {
    printf("%d\n", c.value);
    /**/
  });

  // ASSERT_TRUE(add_vertex(root, 1337));
}
