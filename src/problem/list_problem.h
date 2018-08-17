#ifndef SP_UTIL_LIST_PROBLEM_H
#define SP_UTIL_LIST_PROBLEM_H

namespace prob {
//=====================================
struct SingleList {
  SingleList *next;
  int value;
  SingleList()
      : next{nullptr}
      , value{-1} {
  }
};

struct DoublyList {
  DoublyList *next;
  DoublyList *priv;
  int value;
  DoublyList()
      : next{nullptr}
      , priv{nullptr}
      , value{-1} {
  }
};

struct CircularList {
  CircularList *next;
  CircularList *priv;
  int value;
  CircularList()
      : next{nullptr}
      , priv{nullptr}
      , value{-1} {
  }
};

//=====================================
void
reverse(SingleList &) noexcept;

void
reverse(DoublyList &) noexcept;

void
reverse(CircularList &) noexcept;

} // namespace prob

#endif
