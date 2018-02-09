#ifndef SP_UTIL_LIST_LINKED_LIST_H
#define SP_UTIL_LIST_LINKED_LIST_H

namespace sp {
  namespace impl {
    namespace LinkedList {
      template<typename T>
      struct Node {

      };

    }
  }

template<typename T,template <typename> class Allocator = std::allocator>
struct LinkedList {
  using value_type = T;
  using node_type = impl::LinkedList::Node<T>;
  using SelfType = LinkedList<T,Allocator>;

  node_type *root;
  Allocator<node_type> allocator;

  // explicit LinkedList( const Allocator& alloc ) noexcept;
  LinkedList() noexcept(noexcept(Allocator<node_type>()));

  ~LinkedList() noexcept;

  LinkedList(const SelfType &) = delete;
  LinkedList(const SelfType &&) = delete;

  SelfType& operator=(const SelfType &) = delete;
  SelfType& operator=(const SelfType &&) = delete;
};

template<typename T,template <typename> typename A>
void
swap(LinkedList<T,A>&,LinkedList<T,A>&) noexcept;
/*
 * ==========================================================================
 */
// template<typename T,typename Allocator>
// LinkedList<T,Allocator>::LinkedList( Allocator& alloc ) noexcept 
//   : root {nullptr}
//   , allocator(){
//
// }

template<typename T,template <typename> typename Allocator>
LinkedList<T,Allocator>::LinkedList() noexcept(noexcept(Allocator<node_type>()))
  : root{nullptr}
  , allocator() {
}

template<typename T,template <typename> typename A>
LinkedList<T,A>::~LinkedList() noexcept {
}

template<typename T,template <typename> typename A>
void
swap(LinkedList<T,A>&,LinkedList<T,A>&) noexcept {
}

}

#endif
