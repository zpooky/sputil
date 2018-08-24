#include <graph/Dijkstra.h>
#include <gtest/gtest.h>

TEST(DijkstraTest, test) {
  graph::Vertex<int> v0(0);
  graph::Vertex<int> v1(1);
  graph::Vertex<int> v2(2);
  graph::Vertex<int> v3(3);
  graph::Vertex<int> v4(4);
  ASSERT_TRUE(graph::add_edge(v0, 32, &v1));

  ASSERT_TRUE(graph::dijkstra::shortest_path(&v0, &v1));
  ASSERT_TRUE(graph::dijkstra::shortest_path(&v0, &v0));
  ASSERT_TRUE(graph::dijkstra::shortest_path(&v1, &v1));

  ASSERT_TRUE(graph::add_edge(v1, 10, &v0));
  ASSERT_TRUE(graph::dijkstra::shortest_path(&v1, &v0));

  ASSERT_TRUE(graph::add_edge(v1, 13, &v2));
  ASSERT_TRUE(graph::add_edge(v2, 1, &v1));
  ASSERT_TRUE(graph::add_edge(v1, 9, &v3));

  ASSERT_TRUE(graph::add_edge(v2, 35, &v4));
  ASSERT_TRUE(graph::add_edge(v3, 23, &v4));

  ASSERT_TRUE(graph::dijkstra::shortest_path(&v0, &v4));
  ASSERT_TRUE(graph::dijkstra::shortest_path(&v1, &v4));
  ASSERT_TRUE(graph::dijkstra::shortest_path(&v2, &v4));
  ASSERT_TRUE(graph::dijkstra::shortest_path(&v3, &v4));
}

TEST(DijkstraTest, test_impl2) {
  graph::Vertex<int> v0(0);
  graph::Vertex<int> v1(1);
  graph::Vertex<int> v2(2);
  graph::Vertex<int> v3(3);
  graph::Vertex<int> v4(4);
  ASSERT_TRUE(graph::add_edge(v0, 32, &v1));

  ASSERT_TRUE(graph::dijkstra::shortest_path2(&v0, &v1));
}
