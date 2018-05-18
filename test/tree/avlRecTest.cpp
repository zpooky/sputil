#include <collection/Array.h>
#include <gtest/gtest.h>
#include <tree/avl_rec.h>
#include <util/assert.h>

template <typename T>
static void
a_insert(avl::rec::Tree<T> &tree, T val) {
  printf("insert(tree,'%c')\n", char(val));
  T *res = insert(tree, val);
  dump(tree);

  ASSERT_TRUE(res);
  ASSERT_EQ(*res, val);

  T *fres = find(tree, val);
  ASSERT_TRUE(fres);
  ASSERT_EQ(*fres, val);
  // return res;
  {
    /**/
    ASSERT_TRUE(tree.root);
  }

  printf("\n");
}

template <typename T>
static int
bal(avl::rec::Node<T> *node) {
  assertx(node);
  return avl::rec::impl::balance(node);
}

TEST(avlRecTest, test) {
  using TT = char;
  using namespace avl::rec;

  Tree<TT> tree;

  {
    /*
     * M
     */
    a_insert(tree, 'M');

    Node<TT> *M = tree.root;
    ASSERT_EQ(M->value, 'M');
    ASSERT_EQ(bal(M), 0);
    ASSERT_EQ(M->height, std::size_t(1));
    ASSERT_EQ(nullptr, M->left);
    ASSERT_EQ(nullptr, M->right);
  }
  {
    /*
     * M
     *  \
     *   N
     */
    a_insert(tree, 'N');

    Node<TT> *M = tree.root;
    ASSERT_EQ(M->value, 'M');
    ASSERT_EQ(bal(M), 1);
    ASSERT_EQ(M->height, std::size_t(2));
    {
      /**/
      ASSERT_EQ(nullptr, M->left);
    }
    {
      Node<TT> *N = M->right;
      ASSERT_TRUE(N);
      ASSERT_EQ(N->value, 'N');
      ASSERT_EQ(bal(N), 0);
      ASSERT_EQ(N->height, std::size_t(1));
      ASSERT_EQ(nullptr, N->left);
      ASSERT_EQ(nullptr, N->right);
    }
  }
  {
    /*
     *  N
     * / \
     * M  O
     */
    a_insert(tree, 'O');

    Node<TT> *N = tree.root;
    ASSERT_EQ(N->value, 'N');
    ASSERT_EQ(bal(N), 0);
    ASSERT_EQ(N->height, std::size_t(2));
    {
      Node<TT> *M = N->left;
      ASSERT_TRUE(M);
      ASSERT_EQ(M->value, 'M');
      ASSERT_EQ(bal(M), 0);
      ASSERT_EQ(M->height, std::size_t(1));
      ASSERT_EQ(nullptr, M->left);
      ASSERT_EQ(nullptr, M->right);
    }
    {
      Node<TT> *O = N->right;
      ASSERT_TRUE(O);
      ASSERT_EQ(O->value, 'O');
      ASSERT_EQ(bal(O), 0);
      ASSERT_EQ(O->height, std::size_t(1));
      ASSERT_EQ(nullptr, O->left);
      ASSERT_EQ(nullptr, O->right);
    }
  }
  {
    /*
     *       N
     *      / \
     *      M  O
     *     /
     *     L
     */
    a_insert(tree, 'L');

    Node<TT> *N = tree.root;
    ASSERT_EQ(N->value, 'N');
    ASSERT_EQ(bal(N), -1);
    ASSERT_EQ(N->height, std::size_t(3));
    {
      Node<TT> *M = N->left;
      ASSERT_TRUE(M);
      ASSERT_EQ(M->value, 'M');
      ASSERT_EQ(bal(M), -1);
      ASSERT_EQ(M->height, std::size_t(2));
      {
        Node<TT> *L = M->left;
        ASSERT_TRUE(L);
        ASSERT_EQ(L->value, 'L');
        ASSERT_EQ(bal(L), 0);
        ASSERT_EQ(L->height, std::size_t(1));
        ASSERT_EQ(nullptr, L->left);
        ASSERT_EQ(nullptr, L->right);
      }
      ASSERT_EQ(nullptr, M->right);
    }
    {
      Node<TT> *O = N->right;
      ASSERT_TRUE(O);
      ASSERT_EQ(O->value, 'O');
      ASSERT_EQ(bal(O), 0);
      ASSERT_EQ(O->height, std::size_t(1));
      ASSERT_EQ(nullptr, O->left);
      ASSERT_EQ(nullptr, O->right);
    }
  }
  {
    /*
     *       N
     *      / \
     *      L  O
     *     / \
     *     K  M
     */
    a_insert(tree, 'K');

    Node<TT> *N = tree.root;
    ASSERT_EQ(N->value, 'N');
    ASSERT_EQ(bal(N), -1);
    ASSERT_EQ(N->height, std::size_t(3));
    {
      Node<TT> *L = N->left;
      ASSERT_TRUE(L);
      ASSERT_EQ(L->value, 'L');
      ASSERT_EQ(bal(L), 0);
      ASSERT_EQ(L->height, std::size_t(2));
      {
        Node<TT> *K = L->left;
        ASSERT_TRUE(K);
        ASSERT_EQ(K->value, 'K');
        ASSERT_EQ(bal(K), 0);
        ASSERT_EQ(K->height, std::size_t(1));
        ASSERT_EQ(nullptr, K->left);
        ASSERT_EQ(nullptr, K->right);
      }
      {
        Node<TT> *M = L->right;
        ASSERT_TRUE(M);
        ASSERT_EQ(M->value, 'M');
        ASSERT_EQ(bal(M), 0);
        ASSERT_EQ(M->height, std::size_t(1));
        ASSERT_EQ(nullptr, M->left);
        ASSERT_EQ(nullptr, M->right);
      }
    }
    {
      Node<TT> *O = N->right;
      ASSERT_TRUE(O);
      ASSERT_EQ(O->value, 'O');
      ASSERT_EQ(bal(O), 0);
      ASSERT_EQ(O->height, std::size_t(1));
      ASSERT_EQ(nullptr, O->left);
      ASSERT_EQ(nullptr, O->right);
    }
  }

  {
    /*
     *        N
     *      /   \
     *      L    O
     *     / \    \
     *     K  M    Q
     */
    a_insert(tree, 'Q');

    Node<TT> *N = tree.root;
    ASSERT_EQ(N->value, 'N');
    ASSERT_EQ(bal(N), 0);
    ASSERT_EQ(N->height, std::size_t(3));
    {
      Node<TT> *L = N->left;
      ASSERT_TRUE(L);
      ASSERT_EQ(L->value, 'L');
      ASSERT_EQ(bal(L), 0);
      ASSERT_EQ(L->height, std::size_t(2));
      {
        Node<TT> *K = L->left;
        ASSERT_TRUE(K);
        ASSERT_EQ(K->value, 'K');
        ASSERT_EQ(bal(K), 0);
        ASSERT_EQ(K->height, std::size_t(1));
        ASSERT_EQ(nullptr, K->left);
        ASSERT_EQ(nullptr, K->right);
      }
      {
        Node<TT> *M = L->right;
        ASSERT_TRUE(M);
        ASSERT_EQ(M->value, 'M');
        ASSERT_EQ(bal(M), 0);
        ASSERT_EQ(M->height, std::size_t(1));
        ASSERT_EQ(nullptr, M->left);
        ASSERT_EQ(nullptr, M->right);
      }
    }
    {
      Node<TT> *O = N->right;
      ASSERT_TRUE(O);
      ASSERT_EQ(O->value, 'O');
      ASSERT_EQ(bal(O), 1);
      ASSERT_EQ(O->height, std::size_t(2));
      ASSERT_EQ(nullptr, O->left);
      {
        Node<TT> *Q = O->right;
        ASSERT_TRUE(Q);
        ASSERT_EQ(Q->value, 'Q');
        ASSERT_EQ(bal(Q), 0);
        ASSERT_EQ(Q->height, std::size_t(1));
        ASSERT_EQ(nullptr, Q->left);
        ASSERT_EQ(nullptr, Q->right);
      }
    }
  }
  {
    /*
     *         N
     *      /     \
     *      L      P
     *     / \    / \
     *     K  M   O  Q
     */
    a_insert(tree, 'P');

    Node<TT> *N = tree.root;
    ASSERT_EQ(N->value, 'N');
    ASSERT_EQ(bal(N), 0);
    ASSERT_EQ(N->height, std::size_t(3));
    {
      Node<TT> *L = N->left;
      ASSERT_TRUE(L);
      ASSERT_EQ(L->value, 'L');
      ASSERT_EQ(bal(L), 0);
      ASSERT_EQ(L->height, std::size_t(2));
      {
        Node<TT> *K = L->left;
        ASSERT_TRUE(K);
        ASSERT_EQ(K->value, 'K');
        ASSERT_EQ(bal(K), 0);
        ASSERT_EQ(K->height, std::size_t(1));
        ASSERT_EQ(nullptr, K->left);
        ASSERT_EQ(nullptr, K->right);
      }
      {
        Node<TT> *M = L->right;
        ASSERT_TRUE(M);
        ASSERT_EQ(M->value, 'M');
        ASSERT_EQ(bal(M), 0);
        ASSERT_EQ(M->height, std::size_t(1));
        ASSERT_EQ(nullptr, M->left);
        ASSERT_EQ(nullptr, M->right);
      }
    }
    {
      Node<TT> *P = N->right;
      ASSERT_TRUE(P);
      ASSERT_EQ(P->value, 'P');
      ASSERT_EQ(bal(P), 0);
      ASSERT_EQ(P->height, std::size_t(2));
      {
        Node<TT> *O = P->left;
        ASSERT_TRUE(O);
        ASSERT_EQ(O->value, 'O');
        ASSERT_EQ(bal(O), 0);
        ASSERT_EQ(O->height, std::size_t(1));
        ASSERT_EQ(nullptr, O->left);
        ASSERT_EQ(nullptr, O->right);
      }
      {
        Node<TT> *Q = P->right;
        ASSERT_TRUE(Q);
        ASSERT_EQ(Q->value, 'Q');
        ASSERT_EQ(bal(Q), 0);
        ASSERT_EQ(Q->height, std::size_t(1));
        ASSERT_EQ(nullptr, Q->left);
        ASSERT_EQ(nullptr, Q->right);
      }
    }
  }
  {
    /*
     *         N
     *      /     \
     *      L      P
     *     / \    / \
     *     K  M   O  Q
     *    /
     *    H
     */
    a_insert(tree, 'H');

    Node<TT> *N = tree.root;
    ASSERT_EQ(N->value, 'N');
    ASSERT_EQ(bal(N), -1);
    ASSERT_EQ(N->height, std::size_t(4));
    {
      Node<TT> *L = N->left;
      ASSERT_TRUE(L);
      ASSERT_EQ(L->value, 'L');
      ASSERT_EQ(bal(L), -1);
      ASSERT_EQ(L->height, std::size_t(3));
      {
        Node<TT> *K = L->left;
        ASSERT_TRUE(K);
        ASSERT_EQ(K->value, 'K');
        ASSERT_EQ(bal(K), -1);
        ASSERT_EQ(K->height, std::size_t(2));
        {
          Node<TT> *H = K->left;
          ASSERT_TRUE(H);
          ASSERT_EQ(H->value, 'H');
          ASSERT_EQ(bal(H), 0);
          ASSERT_EQ(H->height, std::size_t(1));
          ASSERT_EQ(nullptr, H->left);
          ASSERT_EQ(nullptr, H->right);
        }
        ASSERT_EQ(nullptr, K->right);
      }
      {
        Node<TT> *M = L->right;
        ASSERT_TRUE(M);
        ASSERT_EQ(M->value, 'M');
        ASSERT_EQ(bal(M), 0);
        ASSERT_EQ(M->height, std::size_t(1));
        ASSERT_EQ(nullptr, M->left);
        ASSERT_EQ(nullptr, M->right);
      }
    }
    {
      Node<TT> *P = N->right;
      ASSERT_TRUE(P);
      ASSERT_EQ(P->value, 'P');
      ASSERT_EQ(bal(P), 0);
      ASSERT_EQ(P->height, std::size_t(2));
      {
        Node<TT> *O = P->left;
        ASSERT_TRUE(O);
        ASSERT_EQ(O->value, 'O');
        ASSERT_EQ(bal(O), 0);
        ASSERT_EQ(O->height, std::size_t(1));
        ASSERT_EQ(nullptr, O->left);
        ASSERT_EQ(nullptr, O->right);
      }
      {
        Node<TT> *Q = P->right;
        ASSERT_TRUE(Q);
        ASSERT_EQ(Q->value, 'Q');
        ASSERT_EQ(bal(Q), 0);
        ASSERT_EQ(Q->height, std::size_t(1));
        ASSERT_EQ(nullptr, Q->left);
        ASSERT_EQ(nullptr, Q->right);
      }
    }
  }
  {
    /*
     *         N
     *      /     \
     *      L      P
     *     / \    / \
     *     I  M   O  Q
     *    / \
     *   H   K
     */
    a_insert(tree, 'I');

    Node<TT> *N = tree.root;
    ASSERT_EQ(N->value, 'N');
    ASSERT_EQ(bal(N), -1);
    ASSERT_EQ(N->height, std::size_t(4));
    {
      Node<TT> *L = N->left;
      ASSERT_TRUE(L);
      ASSERT_EQ(L->value, 'L');
      ASSERT_EQ(bal(L), -1);
      ASSERT_EQ(L->height, std::size_t(3));
      {
        Node<TT> *I = L->left;
        ASSERT_TRUE(I);
        ASSERT_EQ(I->value, 'I');
        ASSERT_EQ(bal(I), 0);
        ASSERT_EQ(I->height, std::size_t(2));
        {
          Node<TT> *H = I->left;
          ASSERT_TRUE(H);
          ASSERT_EQ(H->value, 'H');
          ASSERT_EQ(bal(H), 0);
          ASSERT_EQ(H->height, std::size_t(1));
          ASSERT_EQ(nullptr, H->left);
          ASSERT_EQ(nullptr, H->right);
        }
        {
          Node<TT> *K = I->right;
          ASSERT_TRUE(K);
          ASSERT_EQ(K->value, 'K');
          ASSERT_EQ(K->height, std::size_t(1));
          ASSERT_EQ(bal(K), 0);
          ASSERT_EQ(nullptr, K->left);
          ASSERT_EQ(nullptr, K->right);
        }
      }
      {
        Node<TT> *M = L->right;
        ASSERT_TRUE(M);
        ASSERT_EQ(M->value, 'M');
        ASSERT_EQ(bal(M), 0);
        ASSERT_EQ(M->height, std::size_t(1));
        ASSERT_EQ(nullptr, M->left);
        ASSERT_EQ(nullptr, M->right);
      }
    }
    {
      Node<TT> *P = N->right;
      ASSERT_TRUE(P);
      ASSERT_EQ(P->value, 'P');
      ASSERT_EQ(bal(P), 0);
      ASSERT_EQ(P->height, std::size_t(2));
      {
        Node<TT> *O = P->left;
        ASSERT_TRUE(O);
        ASSERT_EQ(O->value, 'O');
        ASSERT_EQ(bal(O), 0);
        ASSERT_EQ(O->height, std::size_t(1));
        ASSERT_EQ(nullptr, O->left);
        ASSERT_EQ(nullptr, O->right);
      }
      {
        Node<TT> *Q = P->right;
        ASSERT_TRUE(Q);
        ASSERT_EQ(Q->value, 'Q');
        ASSERT_EQ(bal(Q), 0);
        ASSERT_EQ(Q->height, std::size_t(1));
        ASSERT_EQ(nullptr, Q->left);
        ASSERT_EQ(nullptr, Q->right);
      }
    }
  }
  {
    /*
     *        N
     *     /    \
     *     I     P
     *    / \   / \
     *    H  L  O  Q
     *   /  / \
     *  A   K  M
     */
    a_insert(tree, 'A');

    Node<TT> *N = tree.root;
    ASSERT_EQ(N->value, 'N');
    ASSERT_EQ(bal(N), -1);
    ASSERT_EQ(N->height, std::size_t(4));
    {
      Node<TT> *I = N->left;
      ASSERT_TRUE(I);
      ASSERT_EQ(I->value, 'I');
      ASSERT_EQ(bal(I), 0);
      ASSERT_EQ(I->height, std::size_t(3));
      {
        Node<TT> *H = I->left;
        ASSERT_TRUE(H);
        ASSERT_EQ(H->value, 'H');
        ASSERT_EQ(bal(H), -1);
        ASSERT_EQ(H->height, std::size_t(2));
        {
          Node<TT> *A = H->left;
          ASSERT_TRUE(A);
          ASSERT_EQ(A->value, 'A');
          ASSERT_EQ(bal(A), 0);
          ASSERT_EQ(A->height, std::size_t(1));
          ASSERT_EQ(nullptr, A->left);
          ASSERT_EQ(nullptr, A->right);
        }
        ASSERT_EQ(nullptr, H->right);
      }
      {
        Node<TT> *L = I->right;
        ASSERT_TRUE(L);
        ASSERT_EQ(L->value, 'L');
        ASSERT_EQ(bal(L), 0);
        ASSERT_EQ(L->height, std::size_t(2));
        {
          Node<TT> *K = L->left;
          ASSERT_TRUE(K);
          ASSERT_EQ(K->value, 'K');
          ASSERT_EQ(bal(K), 0);
          ASSERT_EQ(K->height, std::size_t(1));
          ASSERT_EQ(nullptr, K->left);
          ASSERT_EQ(nullptr, K->right);
        }
        {
          Node<TT> *M = L->right;
          ASSERT_TRUE(M);
          ASSERT_EQ(M->value, 'M');
          ASSERT_EQ(bal(M), 0);
          ASSERT_EQ(M->height, std::size_t(1));
          ASSERT_EQ(nullptr, M->left);
          ASSERT_EQ(nullptr, M->right);
        }
      }
    }
    {
      Node<TT> *P = N->right;
      ASSERT_TRUE(P);
      ASSERT_EQ(P->value, 'P');
      ASSERT_EQ(bal(P), 0);
      ASSERT_EQ(P->height, std::size_t(2));
      {
        Node<TT> *O = P->left;
        ASSERT_TRUE(O);
        ASSERT_EQ(O->value, 'O');
        ASSERT_EQ(bal(O), 0);
        ASSERT_EQ(O->height, std::size_t(1));
        ASSERT_EQ(nullptr, O->left);
        ASSERT_EQ(nullptr, O->right);
      }
      {
        Node<TT> *Q = P->right;
        ASSERT_TRUE(Q);
        ASSERT_EQ(Q->value, 'Q');
        ASSERT_EQ(bal(Q), 0);
        ASSERT_EQ(Q->height, std::size_t(1));
        ASSERT_EQ(nullptr, Q->left);
        ASSERT_EQ(nullptr, Q->right);
      }
    }
  }
  // MNOL-KQPH-IA
  sp::StaticArray<char, 64> present = {'M', 'N', 'O', 'L', 'K',
                                       'Q', 'P', 'H', 'I', 'A'};
  ASSERT_EQ(std::size_t(10), length(present));
  std::size_t its = 0;
  for_each(present, [&tree, &its](char needle) {
    char *result = find(tree, needle);
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, needle);
    ++its;
  });
  ASSERT_EQ(its, length(present));

  for (std::size_t i = 0; i < length(present); ++i) {
    // printf("i[%zu]\n", i);
    for (std::size_t a = 0; a < i; ++a) {
      char *result = find(tree, present[a]);
      ASSERT_FALSE(result);
    }
    for (std::size_t a = i; a < length(present); ++a) {
      // printf("find(tree,'%c')\n", present[a]);
      char *result = find(tree, present[a]);
      ASSERT_TRUE(result);
      ASSERT_EQ(*result, present[a]);
    }

    printf("remove(tree,'%c')\n", present[i]);
    bool res = remove(tree, present[i]);
    ASSERT_TRUE(res);
    dump(tree);
    printf("\n");
  }

  for_each(present, [&tree](char needle) {
    char *result = find(tree, needle);
    ASSERT_FALSE(result);
  });

  ASSERT_FALSE(tree.root);
}

TEST(avlRecTest, test2) {
  const char start = '0';
  const char end = 'z' + 1;

  using TT = char;
  using namespace avl::rec;

  Tree<TT> tree;
  {
    char it = start;
    while (it != end) {
      char *res = insert(tree, it);
      ASSERT_TRUE(res);
      ASSERT_EQ(it, *res);
      it++;
    }
  }
  dump(tree);

  {
    char it = start;
    while (it != end) {
      for (char a = start; a < it; ++a) {
        char *result = find(tree, a);
        ASSERT_FALSE(result);
      }
      for (char a = it; a < end; ++a) {
        char *result = find(tree, a);
        ASSERT_TRUE(result);
        ASSERT_EQ(*result, a);
      }

      bool res = remove(tree, it++);
      ASSERT_TRUE(res);
    }
  }

  ASSERT_FALSE(tree.root);
}
