#ifndef SP_UTIL_LIST_SKIP_LIST_H
#define SP_UTIL_LIST_SKIP_LIST_H

#include <util/comparator.h>
#include <utility>

/*Sorted container*/
namespace sp {
namespace impl {
namespace SkipList {

template<typename T,std::size_t levels>
struct SkipListNode {
  SkipListNode<T,levels>* next[levels];
  T value;

  template<typename V>
  explicit SkipListNode(V&&v) noexcept
    : next{nullptr}
    , value{std::forward<V>(v)} {
    }
};

}
}

template<typename T,std::size_t levels,typename Comparator = sp::greater>
struct SkipList {
  impl::SkipList::SkipListNode<T,levels> *root;
  SkipList()noexcept
    : root{nullptr} {
    }
};

template <typename T,std::size_t l, typename C, typename V>
T *
insert(SkipList<T,l, C> &, V &&) noexcept;

template <typename T,std::size_t l, typename C, typename K>
T *
find(SkipList<T, l, C> &, const K &) noexcept;

template <typename T,std::size_t l, typename C, typename K>
const T *
find(const SkipList<T, l, C> &, const K &) noexcept;

template <typename T,std::size_t l, typename C, typename K>
T *
remove(SkipList<T, l, C> &, const K &) noexcept;

template <typename T,std::size_t l, typename C>
void swap(SkipList<T,l, C> &,SkipList<T,l, C> &)noexcept;

/*
 * ==========================================================================
 */
namespace impl {
namespace SkipList {

// template<typename V>
// template<typename T,std::size_t l>
// SkipListNode<T,l>::SkipListNode(V&&v) noexcept
// : level{nullptr},
// value{std::forward<V>(v)} {
// }

template <typename T,std::size_t levels, typename Comparator, typename K>
const SkipListNode<T,levels> *
node_for(const sp::SkipList<T, levels, Comparator> &list, const K &needle) noexcept {
  std::size_t level = 0;
  SkipListNode<T,levels> *previous = list.root;
  if(previous){
Lit:
    if(level < levels){
      auto current = previous->next[level];

      if(current){
        constexpr Comparator c;
        if(c(current->value,needle)){

          previous = current;
          goto Lit;
        }else if(c(needle,current->value)){

          ++level;
          goto Lit;
        } else {

          return current;
        }
      } else {
        /* current level: n, we need to continue until node found or reached max level
         * n     ... [previous] -> null
         * n+1   ... [previous] -> [next] -> ...
         * ...
         */
        ++level;
        goto Lit;
      }
    }
  }

  return nullptr;
}

static 
std::size_t random_level(std::size_t max){
  //ffz - find first zero AKA ctz - count trailing zeros

  //TODO does this ensure that level > 0, since level 0 is an ordinary linked
  //list an all nodes should be present there

  //TODO this return the upper limit for which level the node should be
  //included in, for example a node which gets rand_level:3 should be present
  //on level 0,1,2,3...

  //TODO
  // return ffz(rand() & ((1 << max) - 1));
  return 0;
}

}
}

// template<typename T,std::size_t l,template C>
// SkipList<T,l,C>::SkipList()noexcept
//   : root{nullptr} {
// }

template <typename T,std::size_t l, typename C, typename V>
T *
insert(SkipList<T,l, C> &, V &&v) noexcept {
  using namespace impl::SkipList;

  SkipListNode<T,l>*const self = new (std::nothrow) T{std::forward<V>(v)};
  if(self){

    SkipListNode<T,l>*node = nullptr;
    std::size_t level = random_level(l);
    for(std::size_t cLevel=level; cLevel > 0;) {

      /*
       * We do not need to start from the beginning(when != null) since we get
       * more precise result the further up in the list we get
       */
      auto hint = node;
      node = find_previous(hint, level, v);
      if(node){
        /*we update list on this level by inserting self as a link*/
        auto next = node->next[level];
        node->next[level] = self;
        self->next[level] = next;
      }else {
        //TODO empty list
        //TODO or we are new root
      }
    }
    return &self->value;
  }
  return nullptr;
}

template <typename T,std::size_t l, typename C, typename K>
T *
find(SkipList<T, l, C> &list, const K & needle) noexcept {
  const auto & c_l = list;
  return (T*)find(c_l,needle);
}

template <typename T,std::size_t levels, typename Comparator, typename K>
const T *
find(const SkipList<T, levels, Comparator> &list, const K &needle) noexcept {
  using namespace impl::SkipList;
  auto node = node_for(list,needle);
  if(node){
    return &node->value;
  }

  return nullptr;
}//sp::find()

template <typename T,std::size_t l, typename C, typename K>
T *
remove(SkipList<T, l, C> &, const K &) noexcept {
  //find on all level previous and redirect them to -> next
  return nullptr;
}//sp::remove()

template <typename T,std::size_t l, typename C>
void swap(SkipList<T,l, C> &first,SkipList<T,l, C> &second)noexcept {
  std::swap(first.root,second.root);
}

}//namespace sp

#endif
