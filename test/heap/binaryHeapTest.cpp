#include "gtest/gtest.h"

#include <heap/binary.h>
#include <queue>

template<typename C,typename Col,typename Comp>
static void random_fill(heap::Binary<int,C> &heap,std::priority_queue<int,Col,Comp> &ref){
  for(std::size_t i=0;i<heap.capacity;++i){
    ASSERT_EQ(i, heap.length);
    int k=rand();
    int* res  = insert(heap,k);
    ASSERT_FALSE(res ==nullptr);
    ASSERT_EQ(*res,k);
    ref.push(k);
  }
}
//TODO take test

TEST(BinaryHeapTest, MinHeap) {
  constexpr int size = 256;
  int buffer[size]={0};
  heap::MinBinary<int> heap(buffer,size);

  {
    for(int i=0;i<size;++i){
      for(int a=i;a<size;++a){
        ASSERT_FALSE(find(heap,a));
      }
      ASSERT_EQ(std::size_t(heap.length),std::size_t(i));

      int* res = heap::insert(heap, i);
      ASSERT_FALSE(res ==nullptr);
      ASSERT_EQ(*res,i);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres ==nullptr);
      ASSERT_EQ(*pres,0);
      for(int a=0;a<= i;++a){
        int* fres = find(heap,a);
        ASSERT_TRUE(fres);
        ASSERT_EQ(*fres,a);
      }
    }

    {
      int* res = heap::insert(heap, 1);
      ASSERT_TRUE(res ==nullptr);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres ==nullptr);
      ASSERT_EQ(*pres,0);
    }

    for(int i=0;i<size;++i){
      int out=9999999;
      ASSERT_TRUE(heap::take_head(heap,out));
      ASSERT_EQ(i,out);
    }


    {
      int *pres = heap::peek_head(heap);
      ASSERT_TRUE(pres ==nullptr);
    }
  }
  //
  {
    for(int i=size;i-->0;){
      for(int a=i;a-->0;){
        ASSERT_FALSE(find(heap,a));
      }

      int* res = heap::insert(heap, i);
      ASSERT_FALSE(res ==nullptr);
      ASSERT_EQ(*res,i);

      // heap::dump(heap);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres ==nullptr);
      ASSERT_EQ(*pres,i);

      for(int a=size;a-- > i;){
        int* fres = find(heap,a);
        ASSERT_TRUE(fres);
        ASSERT_EQ(*fres,a);
      }
    }

    {
      int* res = heap::insert(heap, 1);
      ASSERT_TRUE(res ==nullptr);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres ==nullptr);
      ASSERT_EQ(*pres,0);
    }

    for(int i=0;i<size;++i){
      int out=9999999;
      ASSERT_TRUE(heap::take_head(heap,out));
      ASSERT_EQ(i,out);
    }


    {
      int *pres = heap::peek_head(heap);
      ASSERT_TRUE(pres ==nullptr);
    }
  }
}

TEST(BinaryHeapTest, MinHeap_random_insert) {
  constexpr int size = 1000;
  int buffer[size]={0};
  heap::MinBinary<int> heap(buffer,size);
  std::priority_queue<int,std::vector<int>,std::greater<int>> ref;

  random_fill(heap,ref);
  ASSERT_EQ(heap.capacity,heap.length);
  ASSERT_EQ(ref.size(),heap.length);
  for(std::size_t i=0;i<size;++i){
    int search = buffer[i];
    int *res = find(heap, search);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res,search);
  }
  // heap::dump(heap);

  int priv = ref.top();
  std::size_t len = heap.length;
  for (std::size_t argh=0; argh < len; ++argh) {
    int r = ref.top();
    int cmp = 0;
    ASSERT_TRUE(take_head(heap, cmp));
    // printf("cmp[%d] == %d\n",cmp,r);
    ASSERT_EQ(r,cmp);
    ASSERT_TRUE(cmp >= priv);
    ref.pop();
    ASSERT_EQ(ref.size(), heap.length);
  }
  heap::dump(heap);

  ASSERT_EQ(std::size_t(0), heap.length);
}

TEST(BinaryHeapTest, MaxHeap_random_insert) {
  constexpr int size = 1000;
  int buffer[size]={0};
  heap::MaxBinary<int> heap(buffer,size);
  std::priority_queue<int,std::vector<int>,std::less<int>> ref;

  random_fill(heap,ref);
  ASSERT_EQ(heap.capacity,heap.length);
  ASSERT_EQ(ref.size(),heap.length);
  for(std::size_t i=0;i<size;++i){
    int search = buffer[i];
    int *res = find(heap, search);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res,search);
  }
  // heap::dump(heap);

  int priv = ref.top();
  std::size_t len = heap.length;
  for (std::size_t argh=0; argh < len; ++argh) {
    int r = ref.top();
    int cmp = 0;
    ASSERT_TRUE(take_head(heap, cmp));
    // printf("cmp[%d] == %d\n",cmp,r);
    ASSERT_EQ(r,cmp);
    ASSERT_TRUE(cmp <= priv);
    ref.pop();
    ASSERT_EQ(ref.size(), heap.length);
  }
  heap::dump(heap);

  ASSERT_EQ(std::size_t(0), heap.length);
}

TEST(BinaryHeapTest, MaxHeap) {
  constexpr int size = 256;
  int buffer[size]={0};
  heap::MaxBinary<int> heap(buffer,size);

  {
    for(int i=0;i<size;++i){
      for(int a=i;a<size;++a){
        ASSERT_FALSE(find(heap,a));
      }

      ASSERT_EQ(std::size_t(heap.length),std::size_t(i));

      int* res = heap::insert(heap, i);
      ASSERT_FALSE(res ==nullptr);
      ASSERT_EQ(*res,i);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres ==nullptr);
      ASSERT_EQ(*pres,i);

      for(int a=0;a<= i;++a){
        int* fres = find(heap,a);
        ASSERT_TRUE(fres);
        ASSERT_EQ(*fres,a);
      }
    }

    {
      int* res = heap::insert(heap, 1);
      ASSERT_TRUE(res ==nullptr);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres ==nullptr);
      ASSERT_EQ(*pres,size-1);
    }

    for(int i=size;i-- > 0;){
      int out=9999999;
      ASSERT_TRUE(heap::take_head(heap,out));
      ASSERT_EQ(i,out);
    }


    {
      int *pres = heap::peek_head(heap);
      ASSERT_TRUE(pres ==nullptr);
    }
  }
  //
  {
    for(int i=size;i-->0;){
      for(int a=i;a-->0;){
        ASSERT_FALSE(find(heap,a));
      }

      int* res = heap::insert(heap, i);
      ASSERT_FALSE(res ==nullptr);
      ASSERT_EQ(*res,i);

      // heap::dump(heap);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres ==nullptr);
      ASSERT_EQ(*pres,size-1);

      for(int a=size;a-- > i;){
        int* fres = find(heap,a);
        ASSERT_TRUE(fres);
        ASSERT_EQ(*fres,a);
      }
    }

    {
      int* res = heap::insert(heap, 1);
      ASSERT_TRUE(res ==nullptr);

      int *pres = heap::peek_head(heap);
      ASSERT_FALSE(pres ==nullptr);
      ASSERT_EQ(*pres,size-1);
    }

    for(int i=size;i-- >0;){
      int out=9999999;
      ASSERT_TRUE(heap::take_head(heap,out));
      ASSERT_EQ(i,out);
    }


    {
      int *pres = heap::peek_head(heap);
      ASSERT_TRUE(pres ==nullptr);
    }
  }
}
