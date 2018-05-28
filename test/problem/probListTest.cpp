#include <gtest/gtest.h>
#include <problem/prob_list.h>
using namespace prob;

template <typename Node>
static void
print_next(Node *l, Node *strt = nullptr, bool first = false) {
  if (first || l != strt) {
    printf("[%d]->", l->value);
    print_next(l->next, strt, false);
  } else {
    printf("\n");
  }
}

template <typename Node>
static void
print_priv(Node *l, Node *strt = nullptr, bool first = false) {
  if (l != strt || first) {
    printf("[%d]<-", l->value);
    print_priv(l->priv, strt, false);
  } else {
    printf("\n");
  }
}

TEST(probListTest, reverse_single_list) {
  SingleList one;
  one.value = 1;

  SingleList two;
  one.next = &two;
  two.value = 2;

  SingleList three;
  two.next = &three;
  three.value = 3;

  SingleList four;
  three.next = &four;
  four.value = 4;
  four.next = nullptr;

  print_next(&one);
  reverse(one);
  print_next(&four);
}

TEST(probListTest, reverse_doubly_list) {
  DoublyList one;
  one.value = 1;
  one.priv = nullptr;

  DoublyList two;
  one.next = &two;
  two.value = 2;
  two.priv = &one;

  DoublyList three;
  two.next = &three;
  three.value = 3;
  three.priv = &two;

  DoublyList four;
  three.next = &four;
  four.value = 4;
  four.priv = &three;
  four.next = nullptr;

  print_next(&one);
  print_priv(&four);

  reverse(one);
  print_next(&four);
  print_priv(&one);
}

TEST(probListTest, reverse_circular_list) {
  CircularList one;
  one.value = 1;

  CircularList two;
  one.next = &two;
  two.value = 2;
  two.priv = &one;

  CircularList three;
  two.next = &three;
  three.value = 3;
  three.priv = &two;

  CircularList four;
  three.next = &four;
  four.value = 4;
  four.priv = &three;

  one.priv = &four;
  four.next = &one;

  print_next(&one, &one, true);
  print_priv(&four, &four, true);
  reverse(one);
  print_next(&four, &four, true);
  print_priv(&one, &one, true);
}
