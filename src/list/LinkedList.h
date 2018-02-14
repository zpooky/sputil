#ifndef SP_UTIL_LIST_LINKED_LIST_H
#define SP_UTIL_LIST_LINKED_LIST_H

#include <memory/Allocator.h>
#include <new>
#include <utility>

namespace sp {
namespace impl {
namespace LinkedList {
template <typename T>
struct Node { //
  Node<T> *next;
  T value;

  template <typename K>
  explicit Node(K &&k) noexcept
      : next{nullptr}
      , value(std::forward<K>(k)) {
  }
};
}
}

template <typename T, template <typename> class Allocator = sp::Allocator>
struct LinkedList {
  using value_type = T;
  using node_type = impl::LinkedList::Node<T>;
  using SelfType = LinkedList<T, Allocator>;

  node_type *root;
  node_type *last;
  Allocator<node_type> allocator;

  // explicit LinkedList( const Allocator& alloc ) noexcept;
  LinkedList() noexcept(noexcept(Allocator<node_type>()));

  ~LinkedList() noexcept;

  LinkedList(const SelfType &) = delete;
  LinkedList(const SelfType &&) = delete;

  SelfType &
  operator=(const SelfType &) = delete;
  SelfType &
  operator=(const SelfType &&) = delete;
};

// TODO emplace

template <typename T, template <typename> typename A>
void
swap(LinkedList<T, A> &, LinkedList<T, A> &) noexcept;

template <typename T, template <typename> typename A>
void
clear(LinkedList<T, A> &) noexcept;

template <typename T, template <typename> typename A>
T *
get(LinkedList<T, A> &, std::size_t) noexcept;

template <typename T, template <typename> typename A>
const T *
get(const LinkedList<T, A> &, std::size_t) noexcept;

template <typename T, template <typename> typename A, typename V>
T *
insert(LinkedList<T, A> &, V &&) noexcept;

template <typename T, template <typename> typename A, typename V>
T *
push_back(LinkedList<T, A> &, V &&) noexcept;

// template <typename T, template <typename> typename A, typename V>
// T *
// push_front(LinkedList<T,A> &, V &&) noexcept;

template <typename T, template <typename> typename A, typename F>
void
for_each(const LinkedList<T, A> &, F) noexcept;

template <typename T, template <typename> typename A, typename F>
void
for_each(LinkedList<T, A> &, F) noexcept;

template <typename T, template <typename> typename A, typename F>
bool
for_all(const LinkedList<T, A> &, F) noexcept;

template <typename T, template <typename> typename A, typename F>
bool
for_all(LinkedList<T, A> &, F) noexcept;

template <typename T, template <typename> typename A, typename F>
T *
find_first(LinkedList<T, A> &, F) noexcept;

template <typename T, template <typename> typename A, typename F>
const T *
find_first(const LinkedList<T, A> &, F) noexcept;

template <typename T, template <typename> typename A, typename F>
bool
remove_first(LinkedList<T, A> &, F) noexcept;

// template <typename Random, typename T>
// void
// shuffle(Random &, sp::LinkedList<T> &) noexcept;
/*
 * =======================================================
 */

namespace impl {
namespace LinkedList {

template <typename T, template <typename> typename A>
const Node<T> *
node_for(const sp::LinkedList<T, A> &l, std::size_t index) noexcept {
  const auto *current = l.root;
Lit:
  if (current) {
    if (index == 0) {
      return current;
    }

    --index;
    current = current->next;
    goto Lit;
  }

  return current;
} // impl::LinkedList::node_for()

template <typename T, template <typename> typename A>
Node<T> *
node_for(sp::LinkedList<T, A> &l, std::size_t index) noexcept {
  const sp::LinkedList<T, A> &c_l = l;
  return (Node<T> *)node_for(c_l, index);
} // impl::LinkedList::node_for()

} // namespace LinkedList
} // namespace impl
/*
 * ==========================================================================
 */
// template<typename T,typename Allocator>
// LinkedList<T,Allocator>::LinkedList( Allocator& alloc ) noexcept
//   : root {nullptr}
//   , allocator(){
//
// }

template <typename T, template <typename> typename Allocator>
LinkedList<T, Allocator>::LinkedList() noexcept(
    noexcept(Allocator<node_type>()))
    : root{nullptr}
    , last{nullptr}
    , allocator() {
}

template <typename T, template <typename> typename A>
LinkedList<T, A>::~LinkedList() noexcept {
Lit:
  if (root) {
    auto *next = root->next;
    allocator.deallocate(root, 1);

    root = next;
    goto Lit;
  }

  root = nullptr;
  last = nullptr;
}

template <typename T, template <typename> typename A>
void
swap(LinkedList<T, A> &first, LinkedList<T, A> &second) noexcept {
  std::swap(first.root, second.root);
  std::swap(first.last, second.last);
  // TODO allocator
}

template <typename T, template <typename> typename A>
void
clear(LinkedList<T, A> &l) noexcept {
  auto *current = l.root;
  auto &allocator = l.allocator;
Lit:
  if (current) {
    auto *next = current->next;

    current->value.~T();
    allocator.deallocate(current, 1);

    current = next;

    goto Lit;
  }

  l.root = nullptr;
  l.last = nullptr;
} // sp::clear()

template <typename T, template <typename> typename A>
T *
get(LinkedList<T, A> &l, std::size_t index) noexcept {
  const LinkedList<T, A> &c_l = l;
  return (T *)get(c_l, index);
} // sp::get()

template <typename T, template <typename> typename A>
const T *
get(const LinkedList<T, A> &l, std::size_t index) noexcept {
  using namespace impl::LinkedList;

  const Node<T> *node = node_for(l, index);
  if (node) {
    return &node->value;
  }

  return nullptr;
} // sp::get()

template <typename T, template <typename> typename A, typename V>
T *
insert(LinkedList<T, A> &list, V &&val) noexcept {
  return push_back(list, std::forward<V>(val));
} // sp::insert()

template <typename T, template <typename> typename A, typename V>
T *
push_back(LinkedList<T, A> &list, V &&val) noexcept {
  using namespace impl::LinkedList;

  auto &allocator = list.allocator;
  Node<T> *node = allocator.allocate(1);
  if (node) {
    ::new (node) Node<T>{std::forward<V>(val)};
    if (!list.root)
      list.root = node;

    if (list.last)
      list.last->next = node;
    list.last = node;

    return &node->value;
  }

  return nullptr;
}

// template <typename T, template <typename> typename A, typename V>
// T *
// push_front(LinkedList<T,A> &, V &&) noexcept {
//   using namespace impl::LinkedList;
//
//   if (list.length < list.capacity) {
//     // Node<T> *const node = node_for(list, list.length);
//     if (node) {
//       T *const value = &node->value;
//
//       value->~T();
//       ::new (value) T{std::forward<V>(val)};
//       list.length++;
//
//       return value;
//     }
//   }
//   return nullptr;
// }

template <typename T, template <typename> typename A, typename F>
void
for_each(const LinkedList<T, A> &list, F f) noexcept {
  using namespace impl::LinkedList;
  const Node<T> *it = list.root;
Lit:
  if (it) {
    const T &value = it->value;
    f(value);

    it = it->next;
    goto Lit;
  }
} // sp::for_each()

template <typename T, template <typename> typename A, typename F>
void
for_each(LinkedList<T, A> &list, F f) noexcept {
  using namespace impl::LinkedList;
  Node<T> *it = list.root;
Lit:
  if (it) {
    T &value = it->value;
    f(value);

    it = it->next;
    goto Lit;
  }
} // sp::for_each()

template <typename T, template <typename> typename A, typename F>
bool
for_all(const LinkedList<T, A> &list, F f) noexcept {
  using namespace impl::LinkedList;
  const Node<T> *it = list.root;
Lit:
  if (it) {
    const T &value = it->value;
    bool r = f(value);
    if (!r) {
      return false;
    }

    it = it->next;
    goto Lit;
  }

  return true;
} // sp::for_all()

template <typename T, template <typename> typename A, typename F>
bool
for_all(LinkedList<T, A> &list, F f) noexcept {
  using namespace impl::LinkedList;
  Node<T> *it = list.root;
Lit:
  if (it) {
    T &value = it->value;
    bool r = f(value);
    if (!r) {
      return false;
    }

    it = it->next;
    goto Lit;
  }

  return true;
} // sp::for_all()

template <typename T, template <typename> typename A, typename F>
T *
find_first(LinkedList<T, A> &list, F f) noexcept {
  using namespace impl::LinkedList;
  Node<T> *it = list.root;
Lit:
  if (it) {
    T &value = it->value;
    if (f(value)) {
      return &it->value;
    }

    it = it->next;
    goto Lit;
  }

  return nullptr;
}

template <typename T, template <typename> typename A, typename F>
const T *
find_first(const LinkedList<T, A> &list, F f) noexcept {
  using namespace impl::LinkedList;
  const Node<T> *it = list.root;
Lit:
  if (it) {
    const T &value = it->value;
    if (f(value)) {
      return &it->value;
    }

    it = it->next;
    goto Lit;
  }

  return nullptr;
}

template <typename T, template <typename> typename A, typename F>
bool
remove_first(LinkedList<T, A> &list, F f) noexcept {
  using namespace impl::LinkedList;

  Node<T> *it = list.root;
  Node<T> *priv = nullptr;
  auto &allocator = list.allocator;

Lit:
  if (it) {
    T &value = it->value;
    if (f(value)) {
      value.~T();

      auto next = it->next;
      if (priv) {
        priv->next = next;
      }

      if (list.root == it) {
        list.root = next;
      }

      allocator.deallocate(it, 1);
      return true;
    }

    priv = it;
    it = it->next;
    goto Lit;
  }

  return false;
} // sp::remove_first()
}

#endif
