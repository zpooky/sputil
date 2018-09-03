#include <graph/Dijkstra.h>
#include <gtest/gtest.h>

TEST(DijkstraTest, test_impl2) {
  graph::Vertex<int> v0(0);
  graph::Vertex<int> v1(1);
  graph::Vertex<int> v2(2);
  graph::Vertex<int> v3(3);
  graph::Vertex<int> v4(4);
  ASSERT_TRUE(graph::add_edge(v0, 32, &v1));
  ASSERT_TRUE(graph::dijkstra::shortest_path(&v0, &v1));

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

TEST(DijkstraTest, test_impl2_asd) {
  graph::Vertex<int> start(0);
  graph::Vertex<int> v1(1);
  graph::Vertex<int> v2(2);
  graph::Vertex<int> v3(3);
  graph::Vertex<int> v4(4);
  graph::Vertex<int> v5(5);
  graph::Vertex<int> v6(6);
  graph::Vertex<int> end(7);

  ASSERT_TRUE(graph::add_edge(start, 6, &end));
  ASSERT_TRUE(graph::add_edge(start, 1, &v1));
  ASSERT_TRUE(graph::add_edge(v1, 1, &v2));
  ASSERT_TRUE(graph::add_edge(v2, 1, &v3));
  ASSERT_TRUE(graph::add_edge(v3, 1, &v4));
  ASSERT_TRUE(graph::add_edge(v4, 1, &v5));
  ASSERT_TRUE(graph::add_edge(v5, 1, &v6));
  ASSERT_TRUE(graph::add_edge(v6, 1, &end));

  ASSERT_TRUE(graph::dijkstra::shortest_path(&start, &end));
}
