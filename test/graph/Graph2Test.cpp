#include <graph/graph.h>
#include <gtest/gtest.h>

TEST(Graph2Test, test) {
  graph::Vertex<int> one(1);
  graph::Vertex<int> two(2);
  ASSERT_FALSE(is_adjacent(one, one));
  ASSERT_FALSE(is_adjacent(one, two));

  ASSERT_TRUE(add_edge(one, 0, &two));
  ASSERT_FALSE(add_edge(one, 0, &two));

  ASSERT_TRUE(is_adjacent(one, two));
}
