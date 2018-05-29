#ifndef SP_UTIL_LIST_PROBLEM_H
#define SP_UTIL_LIST_PROBLEM_H

namespace prob {
struct SingleList {
  SingleList *next;
  int value;
};

struct DoublyList {
  DoublyList *next;
  DoublyList *priv;
  int value;
};

struct CircularList {
  CircularList *next;
  CircularList *priv;
  int value;
};

void
reverse(SingleList &) noexcept;

void
reverse(DoublyList &) noexcept;

void
reverse(CircularList &) noexcept;

} // namespace prob

#endif
