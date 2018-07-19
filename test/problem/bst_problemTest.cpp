#include <gtest/gtest.h>
#include <problem/bst_problem.h>
#include <string>

using namespace bstp;

static void
dump(Tree *tree, std::string prefix = "", bool isTail = true,
     const char *ctx = "") noexcept {
  if (tree) {
    // char name[256] = {0};
    // auto val = std::string(*tree);
    // sprintf(name, "%s%s", ctx, val.c_str());

    printf("%s%s%s[%d]\n", prefix.c_str(), (isTail ? "└──" : "├──"), ctx,
           tree->value);
    const char *ls = (isTail ? "   " : "│  ");
    if (tree->right && tree->left) {
      dump(tree->right, prefix + ls, false, "gt:");
      dump(tree->left, prefix + ls, true, "lt:");
    } else {
      if (tree->left) {
        dump(tree->left, prefix + ls, true, "lt:");
      } else if (tree->right) {
        dump(tree->right, prefix + ls, true, "gt:");
      }
    }
  }
}

TEST(bst_problemTest, balance1) {
  Tree root;
  root.value = 0;
  Tree one;
  one.value = 1;
  Tree two;
  two.value = 2;
  Tree three;
  three.value = 3;
  Tree four;
  four.value = 4;
  Tree five;
  five.value = 5;
  Tree six;
  six.value = 6;

  root.right = &one;
  one.right = &two;
  two.right = &three;
  three.right = &four;
  four.right = &five;
  five.right = &six;

  dump(&root);
  Tree *b = balance(&root);
  dump(b);
}

TEST(bst_problemTest, balance2) {
  Tree zero;
  zero.value = 0;
  Tree one;
  one.value = 1;
  Tree two;
  two.value = 2;
  Tree three;
  three.value = 3;
  Tree four;
  four.value = 4;
  Tree five;
  five.value = 5;
  Tree six;
  six.value = 6;

  one.left = &zero;
  two.left = &one;
  three.left = &two;
  four.left = &three;
  five.left = &four;
  six.left = &five;

  dump(&six);
  Tree *b = balance(&six);
  dump(b);
}

TEST(bst_problemTest, multip) {
  std::size_t max = 12;
  for (std::size_t i = 1; i <= max; ++i) {
    for (std::size_t a = 1; a <= max; ++a) {
      printf("%zu\t", i * a);
    }
    printf("\n");
  }
}
