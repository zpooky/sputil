#include "gtest/gtest.h"
#include <list/SkipList.h>


TEST(SkipListTest, test) {
  constexpr std::size_t levels=7;
  sp::SkipList<int,levels> list;

  constexpr int length = 10;

  for(int i=0;i<length;++i){
    for(int a=0;a<i;++a){
      const int* res = find(list, a);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res,a);
    }

    printf("%d.\n",i);
    int* res = insert(list,i);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res,i);
    dump(list);

    for(int a=i+1;a<length;++a){
      const int* res = find(list,a);
      ASSERT_FALSE(res);
    }
  }
}
