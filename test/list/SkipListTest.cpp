#include "gtest/gtest.h"
#include <list/SkipList.h>
#include <random>


TEST(SkipListTest, test) {
  srand(0);
  constexpr std::size_t levels=7;
  sp::SkipList<int,levels> list;

  constexpr std::size_t length = 112;
  int in[length];
  for(int i=0;i<length;++i){
    in[i]=i;
  }
  std::mt19937 g(0);
  std::shuffle(in, in + length, g);

  for(std::size_t i=0;i<length;++i){
    printf("#%zu\n",i);
    for(std::size_t a=0;a<i;++a){
    // printf("# should be found\n");
      const int* res = find(list, in[a]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[a]);
    }

    int* res = insert(list,in[i]);
    printf("###insert(list,i[%d]): %p\n",in[i],res);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res,in[i]);
    dump(list);

    // printf("# should not be found\n");
    for(std::size_t a=i+1;a<length;++a){
      const int* res = find(list,in[a]);
      // printf("find(list,a[%d]): %p\n",in[a],res);
      ASSERT_FALSE(res);
    }
  }
}
