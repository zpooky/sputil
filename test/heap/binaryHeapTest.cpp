#include <gtest/gtest.h>

#include <collection/Array.h>
#include <heap/binary.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <queue>

template <typename C, typename Col, typename Comp>
static void
random_fill(heap::Binary<int, C> &heap,
            std::priority_queue<int, Col, Comp> &ref) {
  for (std::size_t i = 0; i < heap.capacity; ++i) {
    ASSERT_EQ(i, heap.length);
    int k = rand();
    int *res = insert_lazy(heap, k);
    ASSERT_FALSE(res == nullptr);
    ASSERT_EQ(*res, k);
    ref.push(k);
  }
}

TEST(BinaryHeapTest, MinHeap) {
  constexpr int size = 256;
  int buffer[size] = {0};
  heap::MinBinary<int> heap(buffer, size);

  {
    for (int i = 0; i < size; ++i) {
      for (int a = i; a < size; ++a) {
        ASSERT_FALSE(find(heap, a));
      }
      ASSERT_EQ(std::size_t(heap.length), std::size_t(i));

      int *res = heap::insert_lazy(heap, i);
      ASSERT_FALSE(res == nullptr);
      ASSERT_EQ(*res, i);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres == nullptr);
      ASSERT_EQ(*pres, 0);
      for (int a = 0; a <= i; ++a) {
        int *fres = find(heap, a);
        ASSERT_TRUE(fres);
        ASSERT_EQ(*fres, a);
      }
    }

    {
      int *res = heap::insert_lazy(heap, 1);
      ASSERT_TRUE(res == nullptr);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres == nullptr);
      ASSERT_EQ(*pres, 0);
    }

    for (int i = 0; i < size; ++i) {
      int out = 9999999;
      ASSERT_TRUE(heap::take_head(heap, out));
      ASSERT_EQ(i, out);
    }

    {
      int *pres = heap::peek_head(heap);
      ASSERT_TRUE(pres == nullptr);
    }
  }
  //
  {
    for (int i = size; i-- > 0;) {
      for (int a = i; a-- > 0;) {
        ASSERT_FALSE(find(heap, a));
      }

      int *res = heap::insert_lazy(heap, i);
      ASSERT_FALSE(res == nullptr);
      ASSERT_EQ(*res, i);

      // heap::dump(heap);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres == nullptr);
      ASSERT_EQ(*pres, i);

      for (int a = size; a-- > i;) {
        int *fres = find(heap, a);
        ASSERT_TRUE(fres);
        ASSERT_EQ(*fres, a);
      }
    }

    {
      int *res = heap::insert_lazy(heap, 1);
      ASSERT_TRUE(res == nullptr);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres == nullptr);
      ASSERT_EQ(*pres, 0);
    }

    for (int i = 0; i < size; ++i) {
      int out = 9999999;
      ASSERT_TRUE(heap::take_head(heap, out));
      ASSERT_EQ(i, out);
    }

    {
      int *pres = heap::peek_head(heap);
      ASSERT_TRUE(pres == nullptr);
    }
  }
}

TEST(BinaryHeapTest, MinHeap_random_insert) {
  constexpr int size = 1000;
  int buffer[size] = {0};
  heap::MinBinary<int> heap(buffer, size);
  std::priority_queue<int, std::vector<int>, std::greater<int>> ref;

  random_fill(heap, ref);
  ASSERT_EQ(heap.capacity, heap.length);
  ASSERT_EQ(ref.size(), heap.length);
  for (std::size_t i = 0; i < size; ++i) {
    int search = buffer[i];
    int *res = find(heap, search);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, search);
  }
  // heap::dump(heap);

  int priv = ref.top();
  std::size_t len = heap.length;
  for (std::size_t argh = 0; argh < len; ++argh) {
    int r = ref.top();
    int cmp = 0;
    ASSERT_TRUE(take_head(heap, cmp));
    // printf("cmp[%d] == %d\n",cmp,r);
    ASSERT_EQ(r, cmp);
    ASSERT_TRUE(cmp >= priv);
    ref.pop();
    ASSERT_EQ(ref.size(), heap.length);
  }
  heap::debug::dump(heap);

  ASSERT_EQ(std::size_t(0), heap.length);
}

TEST(BinaryHeapTest, MaxHeap_random_insert) {
  constexpr int size = 1000;
  int buffer[size] = {0};
  heap::MaxBinary<int> heap(buffer, size);
  std::priority_queue<int, std::vector<int>, std::less<int>> ref;

  random_fill(heap, ref);
  ASSERT_EQ(heap.capacity, heap.length);
  ASSERT_EQ(ref.size(), heap.length);
  for (std::size_t i = 0; i < size; ++i) {
    int search = buffer[i];
    int *res = find(heap, search);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, search);
  }
  // heap::dump(heap);

  int priv = ref.top();
  std::size_t len = heap.length;
  for (std::size_t argh = 0; argh < len; ++argh) {
    int r = ref.top();
    int cmp = 0;
    ASSERT_TRUE(take_head(heap, cmp));
    // printf("cmp[%d] == %d\n",cmp,r);
    ASSERT_EQ(r, cmp);
    ASSERT_TRUE(cmp <= priv);
    ref.pop();
    ASSERT_EQ(ref.size(), heap.length);
  }
  heap::debug::dump(heap);

  ASSERT_EQ(std::size_t(0), heap.length);
}

TEST(BinaryHeapTest, MaxHeap) {
  constexpr int size = 256;
  int buffer[size] = {0};
  heap::MaxBinary<int> heap(buffer, size);

  {
    for (int i = 0; i < size; ++i) {
      for (int a = i; a < size; ++a) {
        ASSERT_FALSE(find(heap, a));
      }

      ASSERT_EQ(std::size_t(heap.length), std::size_t(i));

      int *res = heap::insert_lazy(heap, i);
      ASSERT_FALSE(res == nullptr);
      ASSERT_EQ(*res, i);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres == nullptr);
      ASSERT_EQ(*pres, i);

      for (int a = 0; a <= i; ++a) {
        int *fres = find(heap, a);
        ASSERT_TRUE(fres);
        ASSERT_EQ(*fres, a);
      }
    }

    {
      int *res = heap::insert_lazy(heap, 1);
      ASSERT_TRUE(res == nullptr);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres == nullptr);
      ASSERT_EQ(*pres, size - 1);
    }

    for (int i = size; i-- > 0;) {
      int out = 9999999;
      ASSERT_TRUE(heap::take_head(heap, out));
      ASSERT_EQ(i, out);
    }

    {
      int *pres = heap::peek_head(heap);
      ASSERT_TRUE(pres == nullptr);
    }
  }
  //
  {
    for (int i = size; i-- > 0;) {
      for (int a = i; a-- > 0;) {
        ASSERT_FALSE(find(heap, a));
      }

      int *res = heap::insert_lazy(heap, i);
      ASSERT_FALSE(res == nullptr);
      ASSERT_EQ(*res, i);

      // heap::dump(heap);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres == nullptr);
      ASSERT_EQ(*pres, size - 1);

      for (int a = size; a-- > i;) {
        int *fres = find(heap, a);
        ASSERT_TRUE(fres);
        ASSERT_EQ(*fres, a);
      }
    }

    {
      int *res = heap::insert_lazy(heap, 1);
      ASSERT_TRUE(res == nullptr);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres == nullptr);
      ASSERT_EQ(*pres, size - 1);
    }

    for (int i = size; i-- > 0;) {
      int out = 9999999;
      ASSERT_TRUE(heap::take_head(heap, out));
      ASSERT_EQ(i, out);
    }

    {
      int *pres = heap::peek_head(heap);
      ASSERT_TRUE(pres == nullptr);
    }
  }
}

TEST(BinaryHeapTest, test_swap) {
  int *empty = nullptr;
  heap::MinBinary<int> a(empty, 0);
  heap::MinBinary<int> b(empty, 0);
  swap(a, b);
}

TEST(BinaryHeapTest, MaxHeap_lazy) {
  constexpr std::size_t size = 256;
  // const std::size_t it = size * 2;
  heap::StaticMaxBinary<std::size_t, size> heap;
  std::size_t i = 0;
  for (; i < size; ++i) {
    auto res = insert_lazy(heap, i);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);
  }
  for (; i < size * 2; ++i) {
    auto res = insert_lazy(heap, i);
    ASSERT_FALSE(res);
  }

  std::size_t cmp = size;
  while (!is_empty(heap)) {
    std::size_t out = 0;
    ASSERT_TRUE(take_head(heap, out));
    --cmp;
    ASSERT_EQ(cmp, out);
  }
  ASSERT_EQ(cmp, std::size_t(0));
}

TEST(BinaryHeapTest, MaxHeap_lazy_rand) {
  constexpr std::size_t size = 256;
  heap::StaticMaxBinary<std::size_t, size> heap;
  sp::StaticArray<std::size_t, size> in;
  for (std::size_t i = 0; i < capacity(in); ++i) {
    auto res = insert(in, i);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);
  }
  prng::xorshift32 r(1);
  shuffle(r, in);

  ASSERT_EQ(capacity(in), length(in));
  for (std::size_t i = 0; i < length(in); ++i) {
    insert_lazy(heap, in[i]);
  }
  ASSERT_EQ(capacity(heap), length(heap));

  std::size_t cmp = capacity(in);
  while (!is_empty(heap)) {
    std::size_t out = 0;
    ASSERT_TRUE(take_head(heap, out));
    --cmp;
    ASSERT_EQ(cmp, out);
  }
  ASSERT_EQ(cmp, std::size_t(0));
}

TEST(BinaryHeapTest, MaxHeap_eager_check) {
  constexpr int size = 10;
  constexpr int its = size + 10;

  heap::StaticMaxBinary<int, std::size_t(size)> heap;
  int max = 0;
  for (int i = 0; i < its; ++i) {

    auto res = insert_eager(heap, i);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);
    max = i;

    // dump(heap);
    // printf("--------\n");
  }
  ASSERT_EQ(length(heap), capacity(heap));
  int last = 9999999;
  while (!is_empty(heap)) {
    int out = 0;
    ASSERT_TRUE(take_head(heap, out));
    // printf("take_head(heap, out[%d])\n", out);
    ASSERT_TRUE(out < last);
    last = out;
    // ASSERT_EQ(max, out);
    --max;
  }
  ASSERT_TRUE(is_empty(heap));
  ASSERT_EQ(std::size_t(0), length(heap));
}

TEST(BinaryHeapTest, MaxHeap_eager) {
  constexpr int size = 1024;
  heap::StaticMaxBinary<int, size> heap;
  const int it = size * 4;
  int cmp = 0;
  for (int i = 0; i <= it; ++i) {
    // printf("insert_eager(heap, i[%zu])\n", i);
    auto ins = i;
    auto res = insert_eager(heap, ins);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);
    cmp = i;
  }
  // dump(heap);
  while (!is_empty(heap)) {
    int out = 0;
    ASSERT_TRUE(take_head(heap, out));
    // printf("take_head(heap, out[%zu])\n", out);
    ASSERT_EQ(cmp, out);
    --cmp;
  }
  ASSERT_EQ(cmp, it - size);
}

TEST(BinaryHeapTest, MaxHeap_eager_rand) {
  constexpr int size = 1024;
  heap::StaticMaxBinary<int, size> heap;
  sp::StaticArray<int, size * 4> in;
  for (int i = 0; i < int(capacity(in)); ++i) {
    auto res = insert(in, i);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);
  }
  prng::xorshift32 r(1);
  shuffle(r, in);

  ASSERT_EQ(capacity(in), length(in));
  for (int i = 0; i < int(length(in)); ++i) {
    insert_eager(heap, in[i]);
  }
  ASSERT_EQ(capacity(heap), length(heap));

  // dump(heap);
  int cmp = capacity(in);
  while (!is_empty(heap)) {
    int out = 0;
    ASSERT_TRUE(take_head(heap, out));
    // printf("take_head(heap, out[%zu])\n", out);
    --cmp;
    ASSERT_EQ(cmp, out);
  }
}

TEST(BinaryHeapTest, MaxHeap_eager_full) {
  constexpr int size = 10;

  heap::StaticMaxBinary<int, std::size_t(size)> heap;
  for (std::size_t i = 0; i < size; ++i) {

    auto res = insert_eager(heap, 10);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 10);
  }
  ASSERT_TRUE(is_full(heap));

  {
    auto res = insert_eager(heap, 10);
    ASSERT_FALSE(res);
  }
  ASSERT_TRUE(is_full(heap));

  {
    int out = 9999999;
    ASSERT_TRUE(take_head(heap, out));
    ASSERT_EQ(out, 10);
  }
  ASSERT_FALSE(is_full(heap));
  {
    auto res = insert_eager(heap, 9);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 9);
  }
  ASSERT_TRUE(is_full(heap));
  {
    auto res = insert_eager(heap, 10);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 10);
  }
  ASSERT_TRUE(is_full(heap));

  const std::size_t l = length(heap);
  std::size_t cmp = 0;
  while (!is_empty(heap)) {
    int out = 999999999;
    ASSERT_TRUE(take_head(heap, out));
    ASSERT_EQ(out, 10);
    ++cmp;
  }
  ASSERT_EQ(l, cmp);
  ASSERT_TRUE(is_empty(heap));
}

TEST(BinaryHeapTest, MaxHeap_last) {
  constexpr std::size_t size = 1024;
  heap::StaticMaxBinary<std::size_t, size> heap;
  ASSERT_FALSE(last(heap));
  for (std::size_t i = 0; i < size; ++i) {
    {
      auto res = insert_lazy(heap, i);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, i);
    }
    {
      // printf("last()\n");
      auto res = last(heap);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, std::size_t(0));
    }
  }
  std::size_t cmp = size;
  for (std::size_t i = 0; i < size; ++i) {
    {
      // printf("last()\n");
      auto res = last(heap);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, std::size_t(0));
    }
    {
      std::size_t out = 9999999;
      auto res = take_head(heap, out);
      ASSERT_TRUE(res);
      --cmp;
      ASSERT_EQ(cmp, out);
    }
  }
  ASSERT_FALSE(last(heap));
}

template <std::size_t size>
static std::size_t
find_min(sp::StaticArray<std::size_t, size> &in, std::size_t limit) {
  std::size_t result = 99999999;
  for (std::size_t i = 0; i <= limit; ++i) {
    result = std::min(result, in[i]);
  }
  return result;
}

TEST(BinaryHeapTest, MaxHeap_last_rand) {
  constexpr std::size_t size = 1024;

  sp::StaticArray<std::size_t, size> in;
  for (std::size_t i = 0; i < capacity(in); ++i) {
    auto res = insert(in, i);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);
  }
  prng::xorshift32 r(2);
  for (std::size_t seed = 1; seed < 20; ++seed) {
    heap::StaticMaxBinary<std::size_t, size> heap;
    shuffle(r, in);

    ASSERT_EQ(capacity(in), length(in));
    ASSERT_FALSE(last(heap));
    for (std::size_t i = 0; i < length(in); ++i) {
      std::size_t the_min = find_min(in, i);
      {
        auto res = insert_lazy(heap, in[i]);
        ASSERT_TRUE(res);
        // printf("insert_lazy(%zu)\n", in[i]);
        ASSERT_EQ(*res, in[i]);
      }
      {
        auto res = last(heap);
        ASSERT_TRUE(res);
        // printf("last():%zu\n", *res);
        ASSERT_EQ(*res, the_min);
      }
    }
    ASSERT_EQ(capacity(heap), length(heap));
  }
}

TEST(BinaryHeapTest, heapify) {
  prng::xorshift32 r(1);
  {
    constexpr std::size_t size = 1024;
    sp::StaticArray<std::size_t, size> in;
    std::size_t i = 0;
    for (; i < size; ++i) {
      auto res = insert(in, i);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, i);
    }
    shuffle(r, in);
    ASSERT_TRUE(is_full(in));
    auto heap = heap::heapify<std::size_t, sp::greater>(in.data(), length(in));
    ASSERT_TRUE(is_full(heap));
    ASSERT_EQ(size, length(heap));

    while (!is_empty(heap)) {
      std::size_t out = 9999999;
      auto res = take_head(heap, out);
      ASSERT_TRUE(res);
      ASSERT_EQ(--i, out);
      // printf("%zu\n", out);
    }
    ASSERT_EQ(std::size_t(0), i);
    ASSERT_TRUE(is_empty(heap));
  }
}
