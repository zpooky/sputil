#include <tree/red-black.h>
#include "gtest/gtest.h"


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

TEST(red_blackTest, test) {
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
    assert_RED(tree.root->left,10);
    assert_BLACK(tree.root->left->left,5);
    assert_BLACK(tree.root->left->right,15);

    assert_RED(tree.root->right,25);
    assert_BLACK(tree.root->right->left,18);

    assert_BLACK(tree.root->right->right,40);
    assert_RED(tree.root->right->right->right,80);
    ASSERT_TRUE(verify(tree));
  }
}
