#ifndef SP_UTIL_LIST_FIXED_LIST_H
#define SP_UTIL_LIST_FIXED_LIST_H

#include <utility>

namespace sp {

namespace impl {
namespace FixedList {
template <typename T>
struct FixedNode {
  FixedNode<T> *next;
  T value;

  FixedNode()
      : next(nullptr)
      , value() {
  }
};

} // namespace FixedList
} // namespace impl

template <typename T>
struct FixedList {
  impl::FixedList::FixedNode<T> *root;
  impl::FixedList::FixedNode<T> *last;
  std::size_t length;
  std::size_t capacity;

  using value_type = T;

  FixedList();
  FixedList(const FixedList<T> &) = delete;
  FixedList(const FixedList<T> &&) = delete;

  FixedList<T> &
  operator=(const FixedList<T> &) = delete;
  FixedList<T> &
  operator=(const FixedList<T> &&) = delete;

  ~FixedList();
};

template <typename T>
bool
init(FixedList<T> &, std::size_t) noexcept;

template <typename T>
void
swap(FixedList<T> &, FixedList<T> &) noexcept;

template <typename T>
void
clear(FixedList<T> &) noexcept;

template <typename T>
T *
get(FixedList<T> &, std::size_t) noexcept;

template <typename T>
const T *
get(const FixedList<T> &, std::size_t) noexcept;

template <typename T, typename V>
T *
insert(FixedList<T> &, V &&) noexcept;

template <typename T, typename F>
void
for_each(const FixedList<T> &, F) noexcept;

template <typename T, typename F>
bool
for_all(const FixedList<T> &, F) noexcept;

template <typename T, typename F>
bool
remove_first(FixedList<T> &, F) noexcept;

/*
 * =======================================================
 */

namespace impl {
namespace FixedList {

template <typename T>
const FixedNode<T> *
node_for(const sp::FixedList<T> &l, std::size_t index) noexcept {
  if (index < l.capacity) {
    const auto *current = l.root;
  Lstart:
    if (current) {
      if (index == 0) {
        return current;
      }

      --index;
      current = current->next;
      goto Lstart;
    }

    return current;
  }

  return nullptr;
} // impl::FixedList::node_for()

template <typename T>
FixedNode<T> *
node_for(sp::FixedList<T> &l, std::size_t index) noexcept {
  const sp::FixedList<T> &c_l = l;
  return (FixedNode<T> *)node_for(c_l, index);
} // impl::FixedList::node_for()

} // namespace FixedList
} // namespace impl

template <typename T>
FixedList<T>::FixedList()
    : root(nullptr)
    , last(nullptr)
    , length(0)
    , capacity(0) {
}

template <typename T>
FixedList<T>::~FixedList() {
Lstart:
  if (root) {
    auto *next = root->next;
    delete root;

    root = next;
    goto Lstart;
  }

  root = nullptr;
  last = nullptr;
  length = 0;
  capacity = 0;
}

template <typename T>
void
swap(FixedList<T> &first, FixedList<T> &second) noexcept {
  std::swap(first.root, second.root);
  std::swap(first.length, second.length);
  std::swap(first.capacity, second.capacity);
} // sp::swap()

template <typename T>
bool
init(FixedList<T> &l, std::size_t capacity) noexcept {
  using namespace impl::FixedList;
  // auto *next = l.root;

  while (l.capacity < capacity) {
    auto *current = new (std::nothrow) FixedNode<T>;
    if (!current) {
      return false;
    }

    if (!l.root) {
      l.root = current;
    }

    if (l.last) {
      l.last->next = current;
    }
    l.last = current;

    l.capacity++;
  }

  return true;
} // sp::init()

template <typename T>
void
clear(FixedList<T> &l) noexcept {
  auto *current = l.root;
  std::size_t &length = l.length;
Lstart:
  if (current) {
    if (length-- > 0) {
      current->value.~T();
      new (&current->value) T;

      current = current->next;
      goto Lstart;
    }
  }
  length = 0;
} // sp::clear()

template <typename T>
T *
get(FixedList<T> &l, std::size_t index) noexcept {
  const FixedList<T> &c_l = l;
  return (T *)get(c_l, index);
} // sp::get()

template <typename T>
const T *
get(const FixedList<T> &l, std::size_t index) noexcept {
  using namespace impl::FixedList;

  if (index < l.length) {
    const FixedNode<T> *node = node_for(l, index);
    if (node) {
      return &node->value;
    }
  }

  return nullptr;
} // sp::get()

template <typename T, typename V>
T *
insert(FixedList<T> &list, V &&val) noexcept {
  using namespace impl::FixedList;

  if (list.length < list.capacity) {
    FixedNode<T> *const node = node_for(list, list.length);
    if (node) {
      T *const value = &node->value;

      value->~T();
      ::new (value) T{std::forward<V>(val)};
      list.length++;

      return value;
    }
  }

  return nullptr;
} // sp::insert()

template <typename T, typename F>
void
for_each(const FixedList<T> &list, F f) noexcept {
  auto *l = list.root;
  for (std::size_t i = 0; i < list.length; ++i) {
    assert(l);

    f(l->value);
    l = l->next;
  }
} // sp::for_each()

template <typename T, typename F>
bool
for_all(const FixedList<T> &list, F f) noexcept {
  auto *l = list.root;
  for (std::size_t i = 0; i < list.length; ++i) {
    assert(l);

    bool r = f(l->value);
    if (!r) {
      return false;
    }
    l = l->next;
  }
  return true;
} // sp::for_all()

template <typename T, typename F>
bool
remove_first(FixedList<T> &list, F f) noexcept {
  using namespace impl::FixedList;

  auto *current = list.root;
  FixedNode<T> *priv = nullptr;
  for (std::size_t i = 0; i < list.length; ++i) {
    if (f(current->value)) {
      current->value.~T();
      new (&current->value) T;

      auto next = current->next;
      if (priv) {
        priv->next = next;
      }

      if (list.root == current) {
        list.root = next;
      }
      list.length--;

      list.last->next = current;
      list.last = current;

      return true;
    }

    priv = current;
    current = current->next;
  }

  return false;
} // sp::remove_first()

} // namespace sp

#endif
