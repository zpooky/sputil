#include <graph/Undirected.h>
#include <gtest/gtest.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <util/Bitset.h>
#include <utility>

struct StrictGraphTest {
  static std::int64_t active;
  const std::size_t data;

  explicit StrictGraphTest(std::size_t d)
      : data(d) {
    // printf("\nctor %p\n", this);
    assertxs(active >= 0, active);
    ++active;
  }

  StrictGraphTest(const StrictGraphTest &) = delete;

  StrictGraphTest(StrictGraphTest &&o)
      : data(o.data) {
    // printf("\nmove ctor %p <- %p\n", this, &o);
    assertxs(active >= 0, active);
    ++active;
  }

  StrictGraphTest &
  operator=(const StrictGraphTest &&) = delete;
  StrictGraphTest &
  operator=(const StrictGraphTest &) = delete;

  bool
  operator==(std::size_t o) const noexcept {
    return data == o;
  }

  // bool
  // operator>(const StrictGraphTest &o) const noexcept {
  //   return data > o.data;
  // }

  operator std::size_t() const noexcept {
    return data;
  }

  ~StrictGraphTest() {
    // printf("dtor %p\n", this);
    assertxs(active > 0, active);
    --active;
  }
};

std::int64_t StrictGraphTest::active = 0;

/*Parametrized Fixture*/
class GraphTest : public ::testing::Test {
public:
  GraphTest() {
  }

  virtual void
  SetUp() {
    ASSERT_EQ(std::int64_t(0), StrictGraphTest::active);
  }

  virtual void
  TearDown() {
    ASSERT_EQ(std::int64_t(0), StrictGraphTest::active);
  }
};

TEST_F(GraphTest, test) {
  using TT = StrictGraphTest;
  graph::Undirected<TT> first(1);
  auto *second = add_vertex(first, 2);
  {
    ASSERT_TRUE(second);
    ASSERT_EQ(second->value, std::size_t(2));
    ASSERT_TRUE(is_adjacent(first, *second));
    ASSERT_TRUE(is_adjacent(*second, first));
  }
  graph::Undirected<TT> third(3);
  {
    ASSERT_FALSE(is_adjacent(first, third));
    ASSERT_FALSE(is_adjacent(*second, third));

    ASSERT_TRUE(add_edge(first, &third));
    ASSERT_TRUE(is_adjacent(first, third));
    ASSERT_TRUE(is_adjacent(third, first));
    ASSERT_FALSE(is_adjacent(third, *second));
  }
  {
    graph::Undirected<TT> dummy(0);
    ASSERT_FALSE(is_adjacent(first, dummy));
    ASSERT_FALSE(is_adjacent(*second, dummy));
    ASSERT_FALSE(is_adjacent(third, dummy));

    ASSERT_FALSE(remove_edge(first, &dummy));
    ASSERT_FALSE(remove_edge(*second, &dummy));
    ASSERT_FALSE(remove_edge(third, &dummy));
  }
  {
    ASSERT_TRUE(is_adjacent(*second, first));
    ASSERT_TRUE(is_adjacent(first, *second));

    ASSERT_TRUE(remove_edge(first, second));

    ASSERT_FALSE(is_adjacent(*second, first));
    ASSERT_FALSE(is_adjacent(first, *second));

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

TEST_F(GraphTest, test_dtor) {
  using TT = StrictGraphTest;
  using g_type = graph::Undirected<TT, 1024>;

  sp::UinStaticArray<g_type *, g_type::capacity> arr;
  std::size_t a = 0;
  g_type root(a++);

  while (!is_full(arr)) {
    g_type *c = add_vertex(root, a);
    ASSERT_TRUE(c);
    ASSERT_EQ(a, c->value);
    ASSERT_TRUE(insert(arr, c));
    ++a;
  }
  ASSERT_TRUE(is_full(root.edges));

  ASSERT_EQ(a - 1, std::size_t(g_type::capacity));

  for (std::size_t i = 0; i < length(root.edges); ++i) {
    auto &current = root.edges[i];
    ASSERT_TRUE(current.ptr);
    ASSERT_TRUE(is_adjacent(root, *current.ptr));
    ASSERT_TRUE(current.owner);
    {
      graph::Wrapper<g_type> w(&root);
      auto *his = bin_search(current.ptr->edges, w);
      // ASSERT_TRUE(his);
      // ASSERT_EQ(*his, &root);
      // ASSERT_FALSE(his->owner);
    }

    ASSERT_EQ(std::size_t(1), length(current.ptr->edges));
  }

  is_all_adjacent(root);

  {
    prng::xorshift32 r(1);
    for (std::size_t i = 0; i < length(arr); ++i) {
      g_type *current = arr[i];
      const std::size_t max_number = std::min(std::size_t(5), length(arr));
      const std::uint32_t number = uniform_dist(r, 0, max_number);

      std::size_t added = length(current->edges);
      ASSERT_TRUE(added > 0);
      for (std::uint32_t k = 0; k < number; ++k) {
        const std::uint32_t which = uniform_dist(r, 0, length(arr));
        const bool already = is_adjacent(*arr[which], *current);

        // do not try for self assignment
        if (arr[which] != current) {
          const std::size_t l = sp::length(current->edges);
          const bool inserted = add_edge(*current, arr[which]);

          is_all_adjacent(*current);

          // test for no duplicate edges
          ASSERT_EQ(inserted, !already);
          if (inserted) {
            ASSERT_EQ(l + 1, sp::length(current->edges));
            ++added;
          } else {
            ASSERT_EQ(l, sp::length(current->edges));
          }

          ASSERT_TRUE(is_adjacent(*arr[which], *current));
        } else {
          ASSERT_FALSE(already);
        }
      }
      ASSERT_EQ(added, sp::length(current->edges));

      {
        std::size_t l = sp::length(current->edges);
        assertxs(l > 0, l);
      }
    }
  }

  ASSERT_TRUE(is_full(root.edges));

  {
    sp::StaticBitset<g_type::capacity> bits;
    std::size_t bfs_cnt = 0;
    breadth_first(root, [&bfs_cnt, &bits](auto &c) {
      bool priv = sp::set(bits, c.value, true);
      ASSERT_EQ(priv, false);

      // printf("%d\n", c.value);
      ++bfs_cnt;
    });

    for (std::size_t i = 0; i < a; ++i) {
      ASSERT_TRUE(test(bits, i));
    }

    ASSERT_EQ(bfs_cnt, a);
  }

  {
    sp::StaticBitset<g_type::capacity> bits;
    std::size_t dfs_cnt = 0;
    depth_first(root, [&dfs_cnt, &bits](auto &c) {
      bool priv = sp::set(bits, c.value, true);
      assertx(priv == false);

      // printf("%d\n", c.value);
      ++dfs_cnt;
    });

    for (std::size_t i = 0; i < a; ++i) {
      ASSERT_TRUE(test(bits, i));
    }

    ASSERT_EQ(dfs_cnt, a);
  }

  ASSERT_TRUE(is_full(root.edges));

  printf("dtor:s------------------\n");

  // ASSERT_TRUE(add_vertex(root, 1337));
}
