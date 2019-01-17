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
template <typename T>
struct LinkedListNode;
} // namespace impl

//=====================================
template <typename T, template <typename> class Allocator = sp::Allocator>
struct LinkedList {
  using value_type = T;
  using node_type = impl::LinkedListNode<T>;
  using SelfType = LinkedList<T, Allocator>;

  node_type *root;
  node_type *last;
  Allocator<node_type> allocator;

  // explicit LinkedList( const Allocator& alloc ) noexcept;
  LinkedList() noexcept(noexcept(Allocator<node_type>()));

  ~LinkedList() noexcept;

  LinkedList(const SelfType &) = delete;
  LinkedList(SelfType &&) noexcept;

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
template <typename T, template <typename> class A, typename... Arg>
T *
emplace(LinkedList<T, A> &, Arg &&...) noexcept;

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
} // namespace rec

//=====================================
template <typename T, template <typename> class A>
LinkedList<T, A>
take(LinkedList<T, A> &, std::size_t) noexcept;

//=====================================
template <typename T, template <typename> class A>
void
prepend(LinkedList<T, A> &, LinkedList<T, A> &&) noexcept;

//=====================================
// template <typename Random, typename T>
// void
// shuffle(Random &, sp::LinkedList<T> &) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace impl {
template <typename T>
struct LinkedListNode {
  LinkedListNode<T> *next;
  T value;

  template <typename K>
  explicit LinkedListNode(K &&k) noexcept
      : next{nullptr}
      , value(std::forward<K>(k)) {
  }

  template <typename... Arg>
  explicit LinkedListNode(Arg &&... args) noexcept
      : next{nullptr}
      , value(std::forward<Arg>(args)...) {
  }
};

template <typename T, template <typename> class A>
const LinkedListNode<T> *
node_for(const sp::LinkedList<T, A> &l, std::size_t index) noexcept {
  const auto *current = l.root;
  while (current) {
    if (index == 0) {
      return current;
    }

    --index;
    current = current->next;
  }

  return current;
} // impl::LinkedList::node_for()

template <typename T, template <typename> class A>
LinkedListNode<T> *
node_for(sp::LinkedList<T, A> &l, std::size_t index) noexcept {
  const sp::LinkedList<T, A> &c_l = l;
  return (LinkedListNode<T> *)node_for(c_l, index);
} // impl::LinkedList::node_for()

template <typename T, template <typename> class A, typename Predicate>
bool
int_remove(sp::LinkedList<T, A> &self, LinkedListNode<T> *priv,
           LinkedListNode<T> *it, Predicate p) noexcept {
  assertx(it);

  T &value = it->value;
  auto &allocator = self.allocator;

  if (p(value)) {
    auto next = it->next;
    if (priv) {
      priv->next = next;
    }

    if (self.root == it) {
      self.root = next;
    }

    if (self.last == it) {
      self.last = priv;
    }

    it->~LinkedListNode<T>();
    deallocate(allocator, it);
    return true;
  }

  return false;
}
} // namespace impl
//=====================================

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
  while (root) {
    auto *next = root->next;

    root->~LinkedListNode();
    deallocate(allocator, root);

    root = next;
  }

  root = nullptr;
  last = nullptr;
}

template <typename T, template <typename> class Allocator>
LinkedList<T, Allocator>::LinkedList(SelfType &&o) noexcept
    : root{nullptr}
    , last{nullptr}
    , allocator(std::move(o.allocator)) {
  using std::swap;
  swap(root, o.root);
  swap(last, o.last);
}

//=====================================
template <typename T, template <typename> class A>
void
swap(LinkedList<T, A> &first, LinkedList<T, A> &second) noexcept {
  using std::swap;
  swap(first.root, second.root);
  swap(first.last, second.last);
  swap(first.allocator, second.allocator);
}

//=====================================
template <typename T, template <typename> class A>
void
clear(LinkedList<T, A> &l) noexcept {
  using Node = impl::LinkedListNode<T>;

  auto *current = l.root;
  auto &allocator = l.allocator;
Lit:
  if (current) {
    auto *next = current->next;

    current->~Node();
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
  using Node = impl::LinkedListNode<T>;

  const Node *const node = impl::node_for(l, index);
  if (node) {
    return &node->value;
  }

  return nullptr;
} // sp::get()

//=====================================
template <typename T, template <typename> class A, typename V>
T *
insert(LinkedList<T, A> &self, V &&val) noexcept {
  return push_back(self, std::forward<V>(val));
} // sp::insert()

namespace impl {
template <typename T, template <typename> class A>
T *
push_back_x(LinkedList<T, A> &self, LinkedListNode<T> *node) noexcept {
  if (!self.root) {
    self.root = node;
  }

  if (self.last) {
    assertx(self.last->next == nullptr);
    self.last->next = node;
  }
  self.last = node;

  return &node->value;
}

} // namespace impl

template <typename T, template <typename> class A, typename V>
T *
push_back(LinkedList<T, A> &self, V &&val) noexcept {
  using Node = impl::LinkedListNode<T>;

  auto &allocator = self.allocator;
  Node *const node = allocate(allocator);
  if (node) {
    ::new (node) Node{std::forward<V>(val)};
    return impl::push_back_x(self, node);
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
template <typename T, template <typename> class A, typename... Arg>
T *
emplace(LinkedList<T, A> &self, Arg &&... args) noexcept {
  using Node = impl::LinkedListNode<T>;
  auto &allocator = self.allocator;
  Node *const node = allocate(allocator);
  if (node) {
    ::new (node) Node{std::forward<Arg>(args)...};
    if (!self.root) {
      self.root = node;
    }

    if (self.last) {
      assertx(self.last->next == nullptr);
      self.last->next = node;
    }
    self.last = node;

    return &node->value;
  }

  return nullptr;
}

//=====================================
template <typename T, template <typename> class A, typename F>
void
for_each(const LinkedList<T, A> &self, F f) noexcept {
  using Node = impl::LinkedListNode<T>;

  const Node *it = self.root;
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
for_each(LinkedList<T, A> &self, F f) noexcept {
  using Node = impl::LinkedListNode<T>;

  Node *it = self.root;
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
for_all(const LinkedList<T, A> &self, F f) noexcept {
  using Node = impl::LinkedListNode<T>;

  const Node *it = self.root;
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
for_all(LinkedList<T, A> &self, F f) noexcept {
  using Node = impl::LinkedListNode<T>;

  Node *it = self.root;
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
find_first(LinkedList<T, A> &self, F f) noexcept {
  using Node = impl::LinkedListNode<T>;

  Node *it = self.root;
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
find_first(const LinkedList<T, A> &self, F f) noexcept {
  using Node = impl::LinkedListNode<T>;

  const Node *it = self.root;
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
remove_first(LinkedList<T, A> &self, P p) noexcept {
  using Node = impl::LinkedListNode<T>;

  Node *it = self.root;
  Node *priv = nullptr;

Lit:
  if (it) {
    if (int_remove(self, priv, it, p)) {
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
remove_if(LinkedList<T, A> &self, Predicate p) noexcept {
  using Node = impl::LinkedListNode<T>;

  std::size_t result = 0;
  Node *it = self.root;
  Node *priv = nullptr;

Lit:
  if (it) {
    Node *next = it->next;

    if (int_remove(self, priv, it, p)) {
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

namespace rec {
namespace impl {
//=====================================
template <typename T, template <typename> class A>
void
reverse(sp::LinkedList<T, A> &self, sp::impl::LinkedListNode<T> *priv,
        sp::impl::LinkedListNode<T> *current) noexcept {
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
  using Node = sp::impl::LinkedListNode<T>;

  if (self.root) {
    auto root = self.root;

    Node *none = nullptr;
    impl::reverse(self, none, self.root);

    self.last = root;
  }
}
} // namespace rec

//=====================================
namespace impl {
template <typename T, template <typename> class A>
static LinkedListNode<T> *
take_head(LinkedList<T, A> &self) noexcept {
  auto head = self.root;
  if (head) {
    self.root = head->next;
    if (self.last == head) {
      self.last = nullptr;
    }
    head->next = nullptr;
  }

  return head;
}

// template <typename T, template <typename> class A>
// static void
// push_front_x(LinkedList<T, A> &self, LinkedListNode<T> *node) noexcept {
//   if (self.root) {
//     node->next = self.root;
//   }
//   self.root = node;
//
//   if (!self.last) {
//     self.last = node;
//   }
// }
//
} // namespace impl

template <typename T, template <typename> class A>
LinkedList<T, A>
take(LinkedList<T, A> &self, std::size_t n) noexcept {
  LinkedList<T, A> result;
  for (std::size_t i = 0; i < n; ++i) {
    auto current = impl::take_head(self);
    if (current) {
      impl::push_back_x(result, current);
    } else {
      break;
    }
  }

  return result;
}

//=====================================
template <typename T, template <typename> class A>
void
prepend(LinkedList<T, A> &self, LinkedList<T, A> &&other) noexcept {
  auto root = self.root;
  if (other.root) {
    self.root = other.root;
    other.last->next = root;
    self.last = other.last;
  }
  other.root = nullptr;
  other.last = nullptr;
}

//=====================================
} // namespace sp

#endif
