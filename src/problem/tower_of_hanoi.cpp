#include "tower_of_hanoi.h"

namespace sp {
static void
tower_of_hanoi(DynamicStack<int> &from, DynamicStack<int> &to,
               DynamicStack<int> &aux) noexcept {
}

void
tower_of_hanoi(DynamicStack<int> &from, DynamicStack<int> &to) noexcept {
  DynamicStack<int> aux;
  return tower_of_hanoi(from, to, aux);
}

}
