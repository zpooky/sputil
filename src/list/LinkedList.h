#ifndef SP_UTIL_LIST_LINKED_LIST_H
#define SP_UTIL_LIST_LINKED_LIST_H

#include <memory/Allocator.h>
#include <new>
#include <util/assert.h>
#include <utility>

/* ## Stable
 * Nodes in the linked list are stable meaning we can hold a reference to values
 * from the outside and except it to refere to the same value even after other
 * values inserted into the LinkedList.
 */

// TODO emplace

namespace sp {
//=====================================
namespace impl {
namespace LinkedList {
template <typename T>
struct LLNode { //
  LLNode<T> *next;
  T value;

  template <typename K>
  explicit LLNode(K &&k) noexcept
      : next{nullptr}
      , value(std::forward<K>(k)) {
  }
};

} // namespace LinkedList
} // namespace impl

//=====================================
template <typename T, template <typename> class Allocator = sp::Allocator>
struct LinkedList {
  using value_type = T;
  using node_type = impl::LinkedList::LLNode<T>;
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

//=====================================
template <typename T, template <typename> class A>
void
swap(LinkedList<T, A> &, LinkedList<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A>
void
clear(LinkedList<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A>
T *
get(LinkedList<T, A> &, std::size_t) noexcept;

template <typename T, template <typename> class A>
const T *
get(const LinkedList<T, A> &, std::size_t) noexcept;

//=====================================
template <typename T, template <typename> class A, typename V>
T *
insert(LinkedList<T, A> &, V &&) noexcept;

template <typename T, template <typename> class A, typename V>
T *
push_back(LinkedList<T, A> &, V &&) noexcept;

// template <typename T, template <typename> typename A, typename V>
// T *
// push_front(LinkedList<T,A> &, V &&) noexcept;

//=====================================
template <typename T, template <typename> class A, typename F>
void
for_each(const LinkedList<T, A> &, F) noexcept;

template <typename T, template <typename> class A, typename F>
void
for_each(LinkedList<T, A> &, F) noexcept;

//=====================================
template <typename T, template <typename> class A, typename F>
bool
for_all(const LinkedList<T, A> &, F) noexcept;

template <typename T, template <typename> class A, typename F>
bool
for_all(LinkedList<T, A> &, F) noexcept;

//=====================================
template <typename T, template <typename> class A, typename F>
T *
find_first(LinkedList<T, A> &, F) noexcept;

template <typename T, template <typename> class A, typename F>
const T *
find_first(const LinkedList<T, A> &, F) noexcept;

//=====================================
template <typename T, template <typename> class A, typename F>
bool
remove_first(LinkedList<T, A> &, F) noexcept;

//=====================================
template <typename T, template <typename> class A>
bool
is_empty(const LinkedList<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A, typename Predicate>
std::size_t
remove_if(LinkedList<T, A> &, Predicate) noexcept;

template <typename T, template <typename> class A, typename Acum, typename F>
Acum &
reduce(LinkedList<T, A> &, Acum &, F) noexcept;

//=====================================
namespace rec {
template <typename T, template <typename> class A>
void
reverse(LinkedList<T, A> &) noexcept;
}

//=====================================

// template <typename Random, typename T>
// void
// shuffle(Random &, sp::LinkedList<T> &) noexcept;
/*
 * =======================================================
 */

namespace impl {
namespace LinkedList {

template <typename T, template <typename> class A>
const LLNode<T> *
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

template <typename T, template <typename> class A>
LLNode<T> *
node_for(sp::LinkedList<T, A> &l, std::size_t index) noexcept {
  const sp::LinkedList<T, A> &c_l = l;
  return (LLNode<T> *)node_for(c_l, index);
} // impl::LinkedList::node_for()

template <typename T, template <typename> class A, typename Predicate>
bool
int_remove(sp::LinkedList<T, A> &list, LLNode<T> *priv, LLNode<T> *it,
           Predicate p) noexcept {
  assertx(it);

  T &value = it->value;
  auto &allocator = list.allocator;

  if (p(value)) {
    auto next = it->next;
    if (priv) {
      priv->next = next;
    }

    if (list.root == it) {
      list.root = next;
    }

    if (list.last == it) {
      list.last = priv;
    }

    it->~LLNode<T>();
    deallocate(allocator, it);
    return true;
  }

  return false;
}
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

template <typename T, template <typename> class Allocator>
LinkedList<T, Allocator>::LinkedList() noexcept(
    noexcept(Allocator<node_type>()))
    : root{nullptr}
    , last{nullptr}
    , allocator() {
}

template <typename T, template <typename> class A>
LinkedList<T, A>::~LinkedList() noexcept {
Lit:
  if (root) {
    auto *next = root->next;

    root->~LLNode();
    deallocate(allocator, root);

    root = next;
    goto Lit;
  }

  root = nullptr;
  last = nullptr;
}

//=====================================
template <typename T, template <typename> class A>
void
swap(LinkedList<T, A> &first, LinkedList<T, A> &second) noexcept {
  std::swap(first.root, second.root);
  std::swap(first.last, second.last);
  std::swap(first.allocator, second.allocator);
}

//=====================================
template <typename T, template <typename> class A>
void
clear(LinkedList<T, A> &l) noexcept {
  using namespace impl::LinkedList;

  auto *current = l.root;
  auto &allocator = l.allocator;
Lit:
  if (current) {
    auto *next = current->next;

    current->~LLNode<T>();
    deallocate(allocator, current);

    current = next;

    goto Lit;
  }

  l.root = nullptr;
  l.last = nullptr;
} // sp::clear()

//=====================================
template <typename T, template <typename> class A>
T *
get(LinkedList<T, A> &l, std::size_t index) noexcept {
  const LinkedList<T, A> &c_l = l;
  return (T *)get(c_l, index);
} // sp::get()

template <typename T, template <typename> class A>
const T *
get(const LinkedList<T, A> &l, std::size_t index) noexcept {
  using namespace impl::LinkedList;

  const LLNode<T> *node = node_for(l, index);
  if (node) {
    return &node->value;
  }

  return nullptr;
} // sp::get()

//=====================================
template <typename T, template <typename> class A, typename V>
T *
insert(LinkedList<T, A> &list, V &&val) noexcept {
  return push_back(list, std::forward<V>(val));
} // sp::insert()

template <typename T, template <typename> class A, typename V>
T *
push_back(LinkedList<T, A> &list, V &&val) noexcept {
  using namespace impl::LinkedList;

  auto &allocator = list.allocator;
  LLNode<T> *node = allocate(allocator);
  if (node) {
    ::new (node) LLNode<T>{std::forward<V>(val)};
    if (!list.root) {
      list.root = node;
    }

    if (list.last) {
      assertx(list.last->next == nullptr);
      list.last->next = node;
    }
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

//=====================================
template <typename T, template <typename> class A, typename F>
void
for_each(const LinkedList<T, A> &list, F f) noexcept {
  using namespace impl::LinkedList;

  const LLNode<T> *it = list.root;
Lit:
  if (it) {
    const T &value = it->value;
    f(value);

    it = it->next;
    goto Lit;
  }
} // sp::for_each()

template <typename T, template <typename> class A, typename F>
void
for_each(LinkedList<T, A> &list, F f) noexcept {
  using namespace impl::LinkedList;

  LLNode<T> *it = list.root;
Lit:
  if (it) {
    T &value = it->value;
    f(value);

    it = it->next;
    goto Lit;
  }
} // sp::for_each()

//=====================================
template <typename T, template <typename> class A, typename F>
bool
for_all(const LinkedList<T, A> &list, F f) noexcept {
  using namespace impl::LinkedList;

  const LLNode<T> *it = list.root;
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

template <typename T, template <typename> class A, typename F>
bool
for_all(LinkedList<T, A> &list, F f) noexcept {
  using namespace impl::LinkedList;

  LLNode<T> *it = list.root;
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

//=====================================
template <typename T, template <typename> class A, typename F>
T *
find_first(LinkedList<T, A> &list, F f) noexcept {
  using namespace impl::LinkedList;

  LLNode<T> *it = list.root;
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

template <typename T, template <typename> class A, typename F>
const T *
find_first(const LinkedList<T, A> &list, F f) noexcept {
  using namespace impl::LinkedList;

  const LLNode<T> *it = list.root;
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

//=====================================
template <typename T, template <typename> class A, typename P>
bool
remove_first(LinkedList<T, A> &list, P p) noexcept {
  using namespace impl::LinkedList;

  LLNode<T> *it = list.root;
  LLNode<T> *priv = nullptr;

Lit:
  if (it) {
    if (int_remove(list, priv, it, p)) {
      return true;
    }

    priv = it;
    it = it->next;
    goto Lit;
  }

  return false;
} // sp::remove_first()

//=====================================
template <typename T, template <typename> class A>
bool
is_empty(const LinkedList<T, A> &l) noexcept {
  return l.root == nullptr;
}

//=====================================
template <typename T, template <typename> class A, typename Predicate>
std::size_t
remove_if(LinkedList<T, A> &list, Predicate p) noexcept {
  using namespace impl::LinkedList;

  std::size_t result = 0;
  LLNode<T> *it = list.root;
  LLNode<T> *priv = nullptr;

Lit:
  if (it) {
    LLNode<T> *next = it->next;

    if (int_remove(list, priv, it, p)) {
      ++result;
    } else {
      priv = it;
    }
    it = next;

    goto Lit;
  }

  return result;
}

//=====================================
template <typename T, template <typename> class A, typename Acum, typename F>
Acum &
reduce(LinkedList<T, A> &l, Acum &result, F f) noexcept {
  for_each(l, [&result, f](auto &current) {
    /**/
    result = f(result, current);
  });

  return result;
}
//=====================================
namespace rec {
namespace impl {
template <typename T, template <typename> class A>
void
reverse(sp::LinkedList<T, A> &self, sp::impl::LinkedList::LLNode<T> *priv,
        sp::impl::LinkedList::LLNode<T> *current) noexcept {
  if (current) {
    auto next = current->next;
    current->next = priv;
    reverse(self, current, next);
  } else {
    self.root = priv;
  }
}
} // namespace impl

template <typename T, template <typename> class A>
void
reverse(LinkedList<T, A> &self) noexcept {
  if (self.root) {
    auto root = self.root;

    using namespace sp::impl::LinkedList;
    LLNode<T> *none = nullptr;
    impl::reverse(self, none, self.root);

    self.last = root;
  }
}
} // namespace rec

} // namespace sp

#endif
