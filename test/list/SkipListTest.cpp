#include "gtest/gtest.h"
#include <list/SkipList.h>
#include <random>

//TODO test duplicate insert

TEST(SkipListTest, test) {
  constexpr std::size_t levels=7;
  sp::SkipList<int,levels> list;

  constexpr std::size_t length = 20;
  int in[length];
  for(int i=0;i<int(length);++i){
    in[i]=i;
  }
  std::mt19937 g(0);

  for(std::size_t cnt=0;cnt<10;++cnt){
  std::shuffle(in, in + length, g);
  for(std::size_t i=0;i<length;++i){
    // printf("#%zu\n",i);
    for(std::size_t a=0;a<i;++a){
      // printf("# should be found\n");
      const int* res = find(list, in[a]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[a]);
    }

    {
      int* res = insert(list,in[i]);
      // printf("###insert(list,i[wd]): %p\n",in[i],res);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res,in[i]);
      // dump(list);
    }

    // printf("# should not be found\n");
    for(std::size_t a=i+1;a<length;++a){
      const int* res = find(list,in[a]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_FALSE(res);
      ASSERT_FALSE(remove(list,in[a]));
    }
  }

  // printf("#remove\n");
  std::shuffle(in, in + length, g);
  for(std::size_t i=0;i<length;++i){
    for(std::size_t a=0;a<i;++a){
      const int* res = find(list,in[a]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_FALSE(res);
      ASSERT_FALSE(remove(list,in[a]));
    }

    {
      bool res = remove(list,in[i]);
      // printf("remove(list,%d): %s\n",in[i],res ? "true" : "false");
      ASSERT_TRUE(res);
      // dump(list);
    }

    for(std::size_t a=i+1;a<length;++a){
      // printf("# should be found\n");
      const int* res = find(list, in[a]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[a]);
    }
  }
}
}
