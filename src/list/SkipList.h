#ifndef SP_UTIL_LIST_SKIP_LIST_H
#define SP_UTIL_LIST_SKIP_LIST_H

#include <util/comparator.h>
#include <prng/xorshift.h>
#include <utility>
#include <cstring>

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

    ~SkipListNode(){
    }
};

}
}
/*
 * Level 0 should be the plain linked list structure for each higher level the
 * amount of nodes should be halved. This is accomplished by the random level
 * function, by counting in base 2 the trailing zeros which should for each
 * round halve the likelihood of being zero. Thus affectively achieves the
 * desired logic.
 */
template<typename T,std::size_t levels,typename Comparator = sp::greater>
struct SkipList {
  impl::SkipList::SkipListNode<T,levels> *header[levels];
  Xorshift32 state;

  SkipList() noexcept
  : header{nullptr}
  , state{} {
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

template <std::size_t l, typename C>
void
dump(SkipList<int,l,C> &) noexcept;

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

template <typename T,std::size_t levels, typename C>
std::size_t
first_highest(const sp::SkipList<T, levels, C> &list,std::size_t limit) {
  for(std::size_t level=levels;level-- > 0;){
    if(level == limit) {
      return limit;
    }

    if(list.header[level]){
      return level;
    }
  }
  return levels;
}

template <typename T,std::size_t levels, typename C, typename K>
const SkipListNode<T,levels> *
find_node(const sp::SkipList<T, levels, C> &list, const K &needle) noexcept {
  std::size_t level = first_highest(list, levels);
  if(level < levels){
    SkipListNode<T,levels> *current = list.header[level];
    SkipListNode<T,levels> *previous = nullptr;

Lit:
    if(current){
      constexpr C cmp;
      if(cmp(needle, current->value)){// needle > it

        previous = current;
        current = current->next[level];
        goto Lit;
      }else if(cmp(current->value, needle)){//needle < it

        if(level > 0){
          --level;
          if(!previous){
            current = list.header[level];
          }else {
            current = previous;
          }
          previous = nullptr;

          goto Lit;
        }
      } else {

        return current;
      }
    } else {
      if(level > 0){//TODO?
        current = previous;
        --level;
        goto Lit;
      }
    }
  }

  return nullptr;
}


template <typename T,std::size_t L, typename C>
SkipListNode<T,L> *
level_predecessor(SkipListNode<T,L>*start,std::size_t level, const T&needle) noexcept {
  SkipListNode<T,L> *previous = nullptr;
Lit:
  if(start){
    constexpr C cmp;
    if(cmp(start->value,needle)){
      return previous;
    } else {
      previous = start;
      start = start->next[level];
      goto Lit;
    }
  } else {
    return previous;
  }

  return nullptr;
}


static
std::size_t random_level(Xorshift32&state, std::size_t max){
  //ffz - find first zero AKA ctz - count trailing zeros

  //TODO does this ensure that level > 0, since level 0 is an ordinary linked
  //list an all nodes should be present there

  //TODO this return the upper limit for which level the node should be
  //included in, for example a node which gets rand_level:3 should be present
  //on level 0,1,2,3...

/**
* Returns a random level for inserting a new node.
* Hardwired to k=1, p=0.5, max 31 (see above and
* Pugh's "Skip List Cookbook", sec 3.4).
*
*/
  //TODO
  std::uint32_t x = random(state);
  std::size_t level = 0;
  while (((x >>= 1) & 1) != 0){
    ++level;
  }

  return level % max;
}

/*
 * Starting from the highest present node in the list header, search downwards
 * until we are on target_level, then horizontally search for needle node
 * predecessor.
 */
template <typename T,std::size_t L, typename C>
SkipListNode<T,L> *
find_predecessor(sp::SkipList<T,L, C> &list,std::size_t target_level,const T& needle) noexcept {
  /*
   * Limit first to target_level because we need to always to start from tl
   */
  const std::size_t first_level = first_highest(list,target_level);

  SkipListNode<T,L> * previous = nullptr;
  SkipListNode<T,L> * current = nullptr;
  std::size_t level = first_level;
Llevel:
  if(current == nullptr){
    current = list.header[level];
    previous = nullptr;
  }

Lit:
  if(current){
    constexpr C cmp;
    if(cmp(needle,current->value)) {

      if(current->next[level]){
        previous = current;
        current = current->next[level];
        goto Lit;
      }
    } else if(cmp(current->value,needle)){
      current = previous;
      previous = nullptr;
    }
  }
  if(level > target_level){
    --level;
    goto Llevel;
  }

  return current;
}

}
}

// template<typename T,std::size_t l,template C>
// SkipList<T,l,C>::SkipList()noexcept
//   : root{nullptr} {
// }

template <typename T,std::size_t L, typename C, typename V>
T *
insert(SkipList<T,L, C> &list, V &&v) noexcept {
  using namespace impl::SkipList;

  auto self = new (std::nothrow) SkipListNode<T,L>{std::forward<V>(v)};
  if(self){

    const std::size_t target_level = random_level(list.state,L);
    // const std::size_t first_level = first(list, target_level);
    // assert(first_level >= target_level);
    // if(first_level < L){
      auto *start = find_predecessor(list,target_level,self->value);

      for(std::size_t level=target_level+1; level-- > 0;) {
        if(!start){
          start = list.header[level];
        }

        /*
         * We do not need to start from the beginning(when != null) since we get
         * more precise result the further up in the list we get.
         */
        SkipListNode<T,L>*node = level_predecessor<T,L,C>(start, level, self->value);
        if(node){
          /*
           * We update list on this level by inserting self as a link
           */
          auto next = node->next[level];
          node->next[level] = self;
          self->next[level] = next;

          start = node;
        }else {
          /*
           * Empty level or,
           * the first value on this level is greater than self
           */
          auto *next =list.header[level];
          self->next[level]=next;
          list.header[level]= self;
          start = nullptr;
        }
      }

      return &self->value;
    // } else {
    //   #<{(| empty tree |)}>#
    //   for(std::size_t level=target_levels;level-- > 0;) {
    //     assert(list.header[level] == nullptr);
    //     list.header[level] = self;
    //   }
    //   return &self->value;
    // }
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
  auto node = find_node(list,needle);
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

template <std::size_t L, typename C>
void
dump(SkipList<int,L,C> &list) noexcept {
  using namespace impl::SkipList;

  auto index_of = [&list](auto *node) {
    SkipListNode<int,L>*it = list.header[0];
    std::size_t result=0;
    while(it){
      if(node == it){
        return result;
      }
      it=it->next[0];
      ++result;
    }
    return result;
  };
  auto last_index = [&list] {
    SkipListNode<int,L>*it = list.header[0];
    std::size_t result=0;
Lit:
    if(it){
      it = it->next[0];
      result++;
      goto Lit;
    }

    return result;
  };
  constexpr std::size_t b_length = 4;
  auto print_empty = [b_length](std::size_t count) {//
    // printf("count[%zu]\n",count);
    for(std::size_t i=0;i<count;++i){
      printf("+");
      for(std::size_t a=0;a<b_length;++a){
        printf("-");
      }
      printf("-");
    }
    return bool(count > 0);
  };
  auto print_node = [b_length](auto *node,bool first){
    char buffer[b_length];
    std::memset(buffer,' ',b_length);
    auto idx = sprintf(buffer,"%d",node->value);
    buffer[idx] = ' ';
    buffer[b_length-1]='\0';
    if(first){
      printf("[%s]-",buffer);
    }else {
      printf(">[%s]",buffer);
    }
  };

  // if(list.root){
    for(std::size_t level=L;level-- >0;) {
      std::size_t print_index=0;
      SkipListNode<int,L>*it = list.header[level];
      if(level == 0){
        printf("BL |");
      }else {
        printf("L%zu |" ,level);
      }
      bool first=true;
      std::size_t line_length =0;
      while(it){
        std::size_t index = index_of(it);
        // ++print_index;//about to print
        bool printed_empty = print_empty(index-print_index);
        if(first){
          first = !printed_empty;
        }
        print_index=index+1;
        print_node(it,first);
        first = false;
        it = it->next[level];
        ++line_length;
      }

      std::size_t index = last_index();
      print_empty(index-print_index);
      printf(">| %zu node(s)\n",line_length);
    }
  // }
}

}//namespace sp

#endif
