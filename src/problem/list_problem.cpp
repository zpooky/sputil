#include "list_problem.h"
#include <utility>

namespace prob {
//=====================================
// []->[]->[]->nil
// nil<-[]<-[]<-[]
static void
reverse(SingleList *head, SingleList *priv) noexcept {
  if (head) {
    SingleList *next = head->next;
    reverse(next, head);
    head->next = priv;
  }
}

void
reverse(SingleList &l) noexcept {
  return reverse(&l, nullptr);
}

//=====================================
static void
reverse(DoublyList *l) {
  if (l) {
    reverse(l->next);
    std::swap(l->next, l->priv);
  }
}

void
reverse(DoublyList &l) noexcept {
  return reverse(&l);
}

//=====================================
void
reverse(CircularList *start, CircularList *l, bool first) noexcept {
  if (first || l != start) {
    reverse(start, l->next, false);
    std::swap(l->next, l->priv);
  }
}

void
reverse(CircularList &l) noexcept {
  reverse(&l, &l, true);
}

} // namespace prob
