#include <graph/Graph.h>
#include <gtest/gtest.h>

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
  {
    ASSERT_TRUE(remove_edge(first, second));
    ASSERT_FALSE(remove_edge(first, second));
    ASSERT_FALSE(remove_edge(*second, &first));
  }
  {
    ASSERT_TRUE(is_adjacent(third, first));
    ASSERT_TRUE(is_adjacent(first, third));

    ASSERT_TRUE(remove_edge(third, &first));

    ASSERT_FALSE(is_adjacent(third, first));
    ASSERT_FALSE(is_adjacent(first, third));

    ASSERT_FALSE(remove_edge(third, &first));
    ASSERT_FALSE(remove_edge(first, &third));
  }
}
