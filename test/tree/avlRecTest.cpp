#include <collection/Array.h>
#include <gtest/gtest.h>
#include <tree/avl_rec.h>

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
    ASSERT_EQ(M->balance, 0);
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
    ASSERT_EQ(M->balance, 1);
    {
      /**/
      ASSERT_EQ(nullptr, M->left);
    }
    {
      Node<TT> *N = M->right;
      ASSERT_TRUE(N);
      ASSERT_EQ(N->value, 'N');
      ASSERT_EQ(N->balance, 0);
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
    ASSERT_EQ(N->balance, 0);
    {
      Node<TT> *M = N->left;
      ASSERT_TRUE(M);
      ASSERT_EQ(M->value, 'M');
      ASSERT_EQ(M->balance, 0);
      ASSERT_EQ(nullptr, M->left);
      ASSERT_EQ(nullptr, M->right);
    }
    {
      Node<TT> *O = N->right;
      ASSERT_TRUE(O);
      ASSERT_EQ(O->value, 'O');
      ASSERT_EQ(O->balance, 0);
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
    ASSERT_EQ(N->balance, -1);
    {
      Node<TT> *M = N->left;
      ASSERT_TRUE(M);
      ASSERT_EQ(M->value, 'M');
      ASSERT_EQ(M->balance, -1);
      {
        Node<TT> *L = M->left;
        ASSERT_TRUE(L);
        ASSERT_EQ(L->value, 'L');
        ASSERT_EQ(L->balance, 0);
        ASSERT_EQ(nullptr, L->left);
        ASSERT_EQ(nullptr, L->right);
      }
      ASSERT_EQ(nullptr, M->right);
    }
    {
      Node<TT> *O = N->right;
      ASSERT_TRUE(O);
      ASSERT_EQ(O->value, 'O');
      ASSERT_EQ(O->balance, 0);
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
    ASSERT_EQ(N->balance, -1);
    {
      Node<TT> *L = N->left;
      ASSERT_TRUE(L);
      ASSERT_EQ(L->value, 'L');
      ASSERT_EQ(L->balance, 0);
      {
        Node<TT> *K = L->left;
        ASSERT_TRUE(K);
        ASSERT_EQ(K->value, 'K');
        ASSERT_EQ(K->balance, 0);
        ASSERT_EQ(nullptr, K->left);
        ASSERT_EQ(nullptr, K->right);
      }
      {
        Node<TT> *M = L->right;
        ASSERT_TRUE(M);
        ASSERT_EQ(M->value, 'M');
        ASSERT_EQ(M->balance, 0);
        ASSERT_EQ(nullptr, M->left);
        ASSERT_EQ(nullptr, M->right);
      }
    }
    {
      Node<TT> *O = N->right;
      ASSERT_TRUE(O);
      ASSERT_EQ(O->value, 'O');
      ASSERT_EQ(O->balance, 0);
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
    ASSERT_EQ(N->balance, 0);
    {
      Node<TT> *L = N->left;
      ASSERT_TRUE(L);
      ASSERT_EQ(L->value, 'L');
      ASSERT_EQ(L->balance, 0);
      {
        Node<TT> *K = L->left;
        ASSERT_TRUE(K);
        ASSERT_EQ(K->value, 'K');
        ASSERT_EQ(K->balance, 0);
        ASSERT_EQ(nullptr, K->left);
        ASSERT_EQ(nullptr, K->right);
      }
      {
        Node<TT> *M = L->right;
        ASSERT_TRUE(M);
        ASSERT_EQ(M->value, 'M');
        ASSERT_EQ(M->balance, 0);
        ASSERT_EQ(nullptr, M->left);
        ASSERT_EQ(nullptr, M->right);
      }
    }
    {
      Node<TT> *O = N->right;
      ASSERT_TRUE(O);
      ASSERT_EQ(O->value, 'O');
      ASSERT_EQ(O->balance, 1);
      ASSERT_EQ(nullptr, O->left);
      {
        Node<TT> *Q = O->right;
        ASSERT_TRUE(Q);
        ASSERT_EQ(Q->value, 'Q');
        ASSERT_EQ(Q->balance, 0);
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
    ASSERT_EQ(N->balance, 0);
    {
      Node<TT> *L = N->left;
      ASSERT_TRUE(L);
      ASSERT_EQ(L->value, 'L');
      ASSERT_EQ(L->balance, 0);
      {
        Node<TT> *K = L->left;
        ASSERT_TRUE(K);
        ASSERT_EQ(K->value, 'K');
        ASSERT_EQ(K->balance, 0);
        ASSERT_EQ(nullptr, K->left);
        ASSERT_EQ(nullptr, K->right);
      }
      {
        Node<TT> *M = L->right;
        ASSERT_TRUE(M);
        ASSERT_EQ(M->value, 'M');
        ASSERT_EQ(M->balance, 0);
        ASSERT_EQ(nullptr, M->left);
        ASSERT_EQ(nullptr, M->right);
      }
    }
    {
      Node<TT> *P = N->right;
      ASSERT_TRUE(P);
      ASSERT_EQ(P->value, 'P');
      ASSERT_EQ(P->balance, 0);
      {
        Node<TT> *O = P->left;
        ASSERT_TRUE(O);
        ASSERT_EQ(O->value, 'O');
        ASSERT_EQ(O->balance, 0);
        ASSERT_EQ(nullptr, O->left);
        ASSERT_EQ(nullptr, O->right);
      }
      {
        Node<TT> *Q = P->right;
        ASSERT_TRUE(Q);
        ASSERT_EQ(Q->value, 'Q');
        ASSERT_EQ(Q->balance, 0);
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
    ASSERT_EQ(N->balance, -1);
    {
      Node<TT> *L = N->left;
      ASSERT_TRUE(L);
      ASSERT_EQ(L->value, 'L');
      ASSERT_EQ(L->balance, -1);
      {
        Node<TT> *K = L->left;
        ASSERT_TRUE(K);
        ASSERT_EQ(K->value, 'K');
        ASSERT_EQ(K->balance, -1);
        {
          Node<TT> *H = K->left;
          ASSERT_TRUE(H);
          ASSERT_EQ(H->value, 'H');
          ASSERT_EQ(H->balance, 0);
          ASSERT_EQ(nullptr, H->left);
          ASSERT_EQ(nullptr, H->right);
        }
        ASSERT_EQ(nullptr, K->right);
      }
      {
        Node<TT> *M = L->right;
        ASSERT_TRUE(M);
        ASSERT_EQ(M->value, 'M');
        ASSERT_EQ(M->balance, 0);
        ASSERT_EQ(nullptr, M->left);
        ASSERT_EQ(nullptr, M->right);
      }
    }
    {
      Node<TT> *P = N->right;
      ASSERT_TRUE(P);
      ASSERT_EQ(P->value, 'P');
      ASSERT_EQ(P->balance, 0);
      {
        Node<TT> *O = P->left;
        ASSERT_TRUE(O);
        ASSERT_EQ(O->value, 'O');
        ASSERT_EQ(O->balance, 0);
        ASSERT_EQ(nullptr, O->left);
        ASSERT_EQ(nullptr, O->right);
      }
      {
        Node<TT> *Q = P->right;
        ASSERT_TRUE(Q);
        ASSERT_EQ(Q->value, 'Q');
        ASSERT_EQ(Q->balance, 0);
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
    ASSERT_EQ(N->balance, -1);
    {
      Node<TT> *L = N->left;
      ASSERT_TRUE(L);
      ASSERT_EQ(L->value, 'L');
      ASSERT_EQ(L->balance, -1);
      {
        Node<TT> *I = L->left;
        ASSERT_TRUE(I);
        ASSERT_EQ(I->value, 'I');
        ASSERT_EQ(I->balance, 0);
        {
          Node<TT> *H = I->left;
          ASSERT_TRUE(H);
          ASSERT_EQ(H->value, 'H');
          ASSERT_EQ(H->balance, 0);
          ASSERT_EQ(nullptr, H->left);
          ASSERT_EQ(nullptr, H->right);
        }
        {
          Node<TT> *K = I->right;
          ASSERT_TRUE(K);
          ASSERT_EQ(K->value, 'K');
          ASSERT_EQ(K->balance, 0);
          ASSERT_EQ(nullptr, K->left);
          ASSERT_EQ(nullptr, K->right);
        }
      }
      {
        Node<TT> *M = L->right;
        ASSERT_TRUE(M);
        ASSERT_EQ(M->value, 'M');
        ASSERT_EQ(M->balance, 0);
        ASSERT_EQ(nullptr, M->left);
        ASSERT_EQ(nullptr, M->right);
      }
    }
    {
      Node<TT> *P = N->right;
      ASSERT_TRUE(P);
      ASSERT_EQ(P->value, 'P');
      ASSERT_EQ(P->balance, 0);
      {
        Node<TT> *O = P->left;
        ASSERT_TRUE(O);
        ASSERT_EQ(O->value, 'O');
        ASSERT_EQ(O->balance, 0);
        ASSERT_EQ(nullptr, O->left);
        ASSERT_EQ(nullptr, O->right);
      }
      {
        Node<TT> *Q = P->right;
        ASSERT_TRUE(Q);
        ASSERT_EQ(Q->value, 'Q');
        ASSERT_EQ(Q->balance, 0);
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
    ASSERT_EQ(N->balance, -1);
    {
      Node<TT> *I = N->left;
      ASSERT_TRUE(I);
      ASSERT_EQ(I->value, 'I');
      ASSERT_EQ(I->balance, 0);
      {
        Node<TT> *H = I->left;
        ASSERT_TRUE(H);
        ASSERT_EQ(H->value, 'H');
        ASSERT_EQ(H->balance, -1);
        {
          Node<TT> *A = H->left;
          ASSERT_TRUE(A);
          ASSERT_EQ(A->value, 'A');
          ASSERT_EQ(A->balance, 0);
          ASSERT_EQ(nullptr, A->left);
          ASSERT_EQ(nullptr, A->right);
        }
        ASSERT_EQ(nullptr, H->right);
      }
      {
        Node<TT> *L = I->right;
        ASSERT_TRUE(L);
        ASSERT_EQ(L->value, 'L');
        ASSERT_EQ(L->balance, 0);
        {
          Node<TT> *K = L->left;
          ASSERT_TRUE(K);
          ASSERT_EQ(K->value, 'K');
          ASSERT_EQ(K->balance, 0);
          ASSERT_EQ(nullptr, K->left);
          ASSERT_EQ(nullptr, K->right);
        }
        {
          Node<TT> *M = L->right;
          ASSERT_TRUE(M);
          ASSERT_EQ(M->value, 'M');
          ASSERT_EQ(M->balance, 0);
          ASSERT_EQ(nullptr, M->left);
          ASSERT_EQ(nullptr, M->right);
        }
      }
    }
    {
      Node<TT> *P = N->right;
      ASSERT_TRUE(P);
      ASSERT_EQ(P->value, 'P');
      ASSERT_EQ(P->balance, 0);
      {
        Node<TT> *O = P->left;
        ASSERT_TRUE(O);
        ASSERT_EQ(O->value, 'O');
        ASSERT_EQ(O->balance, 0);
        ASSERT_EQ(nullptr, O->left);
        ASSERT_EQ(nullptr, O->right);
      }
      {
        Node<TT> *Q = P->right;
        ASSERT_TRUE(Q);
        ASSERT_EQ(Q->value, 'Q');
        ASSERT_EQ(Q->balance, 0);
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
  }

  for_each(present, [&tree](char needle) {
    char *result = find(tree, needle);
    ASSERT_FALSE(result);
  });
}
