#include "tower_of_hanoi.h"
#include <sort/util.h>
#include <util/assert.h>

namespace sp {

static void
tower_of_hanoi(DynamicStack<int> &from, DynamicStack<int> &to,
               DynamicStack<int> &aux) noexcept {
}

void
tower_of_hanoi(DynamicStack<int> &from, DynamicStack<int> &to) noexcept {
  assertx_f({
    const int *priv = peek(from);
    for_each(from, [&priv](const int &asd) {
      /**/
      assertx(priv);
      assertxs(*priv >= asd, *priv, asd);
      priv = &asd;
    });
  });

  DynamicStack<int> aux;
  return tower_of_hanoi(from, to, aux);
}
}
