#include <tree/red-black.h>
#include "gtest/gtest.h"
#include <random>


#define ASSERT_NODE(NODE,CMP,COLOUR) \
  do { \
    ASSERT_TRUE(NODE); \
    ASSERT_EQ(NODE->colour,COLOUR); \
    ASSERT_EQ(NODE->value,CMP); \
  } while(false)

#define assert_BLACK(NODE,CMP) ASSERT_NODE(NODE,CMP,rb::Colour::BLACK)
#define assert_RED(NODE,CMP) ASSERT_NODE(NODE,CMP,rb::Colour::RED)

template<typename T>
static void assert_insert(rb::Tree<T>&tree,const T&v) {
  printf("insert(tree,%d)\n",v);
  auto res = insert(tree,v);
  ASSERT_TRUE(std::get<0>(res));
  ASSERT_TRUE(std::get<1>(res));
  // return std::get<0>(res);
}

static std::size_t verify_number_BLACK(rb::Node<int>*tree) {
  if(tree == nullptr){
    return 1;
  }

  std::size_t left = verify_number_BLACK(tree->left);
  std::size_t right = verify_number_BLACK(tree->right);

  return left+right;
}

TEST(red_blackTest, test1) {
  rb::Tree<int> tree;
  printf("1. ");
  {
    int val=8;
    assert_insert(tree,val);
    assert_BLACK(tree.root,val);
    ASSERT_TRUE(verify(tree));
  }
  printf("2. ");
  {
    int val=18;
    assert_insert(tree,val);
    assert_BLACK(tree.root,8);
    assert_RED(tree.root->right,val);
    ASSERT_TRUE(verify(tree));
  }
  printf("3. ");
  {
    int val=5;
    assert_insert(tree,val);
    assert_BLACK(tree.root,8);
    assert_RED(tree.root->right,18);
    assert_RED(tree.root->left,val);
    ASSERT_TRUE(verify(tree));
  }
  printf("4. ");
  {
    int val=15;
    assert_insert(tree,val);
    assert_BLACK(tree.root,8);
    assert_BLACK(tree.root->left,5);

    assert_BLACK(tree.root->right,18);
    assert_RED(tree.root->right->left,val);
    ASSERT_TRUE(verify(tree));
  }
  printf("5. ");
  {
    assert_insert(tree,17);
    dump(tree);
    assert_BLACK(tree.root,8);
    assert_BLACK(tree.root->left,5);

    assert_BLACK(tree.root->right,17);
    assert_RED(tree.root->right->left,15);
    assert_RED(tree.root->right->right,18);
    if(!verify(tree)) {
      dump(tree);
      ASSERT_FALSE(true);
    }
  }
  printf("6. ");
  {
    assert_insert(tree,25);
    assert_BLACK(tree.root,8);
    assert_BLACK(tree.root->left,5);

    assert_RED(tree.root->right,17);
    assert_BLACK(tree.root->right->left,15);

    assert_BLACK(tree.root->right->right,18);
    assert_RED(tree.root->right->right->right,25);
    ASSERT_TRUE(verify(tree));
    dump(tree);
  }
  printf("7. ");
  {
    assert_insert(tree,40);
    dump(tree);
    assert_BLACK(tree.root,8);
    assert_BLACK(tree.root->left,5);

    assert_RED(tree.root->right,17);
    assert_BLACK(tree.root->right->left,15);

    assert_BLACK(tree.root->right->right,25);
    assert_RED(tree.root->right->right->left,18);
    assert_RED(tree.root->right->right->right,40);
    ASSERT_TRUE(verify(tree));
  }

  printf("8. ");
  {
    assert_insert(tree,80);
    dump(tree);
    assert_BLACK(tree.root,17);
    assert_RED(tree.root->left,8);
    assert_BLACK(tree.root->left->left,5);
    assert_BLACK(tree.root->left->right,15);

    assert_RED(tree.root->right,25);
    assert_BLACK(tree.root->right->left,18);

    assert_BLACK(tree.root->right->right,40);
    assert_RED(tree.root->right->right->right,80);
    ASSERT_TRUE(verify(tree));
  }

  // std::size_t left = verify_number_BLACK(tree.root->left);
  // std::size_t right = verify_number_BLACK(tree.root->right);
  // ASSERT_EQ(left,right);
}


TEST(red_blackTest, test2) {
  rb::Tree<int> tree;
  for(int i=9 ;i>= 0;--i){
    insert(tree,i);
  }
  {
    auto root = tree.root;
    assert_BLACK(root,6);
    assert_BLACK(root->right,8);
    assert_BLACK(root->right->left,7);
    assert_BLACK(root->right->right,9);

    assert_BLACK(root->left,4);
    assert_BLACK(root->left->right,5);

    assert_RED(root->left->left,2);
    assert_BLACK(root->left->left->right,3);
    assert_BLACK(root->left->left->left,1);
    assert_RED(root->left->left->left->left,0);
  }
}
template<typename T,std::size_t s>
static std::size_t size_arr(T (&)[s]){
  return s;
}

TEST(red_blackTest, test3) {
  // std::random_device rd;
  // std::mt19937 g(rd());
  //
  // constexpr int in_size=20;
  // int in[in_size];
  // for(int i=0;i<in_size;++i){
  //   in[i]=i;
  // }
  // std::shuffle(in, in + in_size, g);
  // printf("{");
  // for(int i=0;i<in_size;++i){
  //   printf("%d,",in[i]);
  // }
  // printf("}\n");

  rb::Tree<int> tree;
  int in[] = {16,4,13,11,7,3,14,9,8,17,5,19,6,1};
  for(std::size_t i=0;i<size_arr(in);++i){
    insert(tree,in[i]);
  }
  {
    auto root = tree.root;
    assert_BLACK(root,13);
    {
      assert_BLACK(root->right,16);
      assert_BLACK(root->right->left,14);
      assert_BLACK(root->right->right,17);
      assert_RED(root->right->right->right,19);
    }
    {
      assert_BLACK(root->left,7);
      assert_BLACK(root->left->right,9);
      assert_RED(root->left->right->left,8);
      assert_RED(root->left->right->right,11);
    }
    {
      assert_RED(root->left->left,4);
      assert_BLACK(root->left->left->left,3);
      assert_RED(root->left->left->left->left,1);
      assert_BLACK(root->left->left->right,5);
      assert_RED(root->left->left->right->right,6);

    }
  }

}
