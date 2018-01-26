#ifndef SP_UTIL_LIST_SKIP_LIST_H
#define SP_UTIL_LIST_SKIP_LIST_H

#include <util/comparator.h>
#include <prng/xorshift.h>
#include <utility>
#include <cstring>

/*
 * TODO refactor to use SkipListNode as a header this will reduce the amount of
 * special cases.
 */

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

  ~SkipList() noexcept {
    auto*it = header[0];
    while(it){
      auto*next = it->next[0];
      delete it;
      it = next;
    }

    for(std::size_t i=0;i<levels;++i){
      header[i] = nullptr;
    }
  }

  static_assert(levels > 0, "gt 0");
  static_assert(levels < 32, "lt 32");
};

//TODO SkipListSet

template <typename T,std::size_t l, typename C, typename V>
T *
insert(SkipList<T,l, C> &, V &&) noexcept;

template <typename T,std::size_t l, typename C, typename V>
std::tuple<T *, bool>
insert_unique(SkipList<T,l, C> &, V &&) noexcept;

template <typename T,std::size_t l, typename C, typename K>
T *
find(SkipList<T, l, C> &, const K &) noexcept;

template <typename T,std::size_t l, typename C, typename K>
const T *
find(const SkipList<T, l, C> &, const K &) noexcept;

template <typename T,std::size_t l, typename C,typename K>
bool take(SkipList<T, l, C> &,  const K&, T &) noexcept;

template <typename T,std::size_t l, typename C, typename K>
bool
remove(SkipList<T, l, C> &, const K &) noexcept;

template <typename T,std::size_t l, typename C>
bool
is_empty(const SkipList<T, l, C> &) noexcept;

template <typename T,std::size_t l, typename C>
void swap(SkipList<T,l, C> &,SkipList<T,l, C> &)noexcept;

template <std::size_t l, typename C>
void
dump(const SkipList<int,l,C> &) noexcept;

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

/*
 * Randomize the depth of the about-to-inserted node
 */
static
std::size_t random_level(Xorshift32&state, std::size_t max){
  /*
   * Generates a random number
   * Bitwise count the amount of leading zeroes <--
   * Limit the count to max
   *
   * For each level of bit compare we get a 50/50 chance that the bit is 2/0.
   * This mirrors the requirement of the Skip List that each level contains
   * half the amount of nodes as its preceding level.
   *
   * ...
   * L2 | 25%
   * L1 | 50%
   * L0 | 100%
   */

  //TODO verify this is correct
  std::uint32_t x = random(state);
  std::size_t level = 0;
  while (((x >>= 1) & 1) != 0){
    ++level;
  }

  return level % max;
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
      } else { // needle == it

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

template <typename T,std::size_t L, typename C,typename K>
SkipListNode<T,L> *
find_level_predecessor(SkipListNode<T,L>*start,std::size_t level, const K&needle) noexcept {
  SkipListNode<T,L> *previous = nullptr;
Lit:
  if(start){
    constexpr C cmp;
    if(cmp(start->value,needle)){ //it > needle = return priv
      return previous;
    } else if(cmp(needle,start->value)){// it < needle = continue
      previous = start;
      start = start->next[level];
      goto Lit;
    } else {//exact match
      return previous;
    }
  } else {
    return previous;
  }

  return nullptr;
}

/*
 * Starting from the highest present node in the list header, search downwards
 * until we are on target_level, then horizontally search for needle node
 * predecessor.
 */
template <typename T,std::size_t L, typename C, typename K>
SkipListNode<T,L> *
find_predecessor(sp::SkipList<T,L, C> &list,std::size_t target_level,const K& needle) noexcept {
  SkipListNode<T,L> * previous = nullptr;
  SkipListNode<T,L> * current = nullptr;
  std::size_t level = L-1;
Llevel:
  if(!current){
    current = list.header[level];
    previous = nullptr;
  }

Lhorizontal:
  if(current){
    constexpr C cmp;
    if(cmp(needle,current->value)) {// needle > it

      if(current->next[level]){
        previous = current;
        current = current->next[level];
        goto Lhorizontal;
      }
    } else if(cmp(current->value,needle)){ // needle < it
      current = previous;
      previous = nullptr;
    }else {
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

template <typename T,std::size_t L, typename C, typename K>
static bool 
is_equal(const SkipListNode<T,L>*node,const K&needle) {
  if(node){
    constexpr C cmp;
    if(!cmp(needle,node->value) && !cmp(node->value,needle)){
      return true;
    }
  }

  return false;
}

template <typename T,std::size_t L, typename C, typename K>
static bool 
is_next_equal(const SkipListNode<T,L>*node,const K&needle,std::size_t level) {
  if(node){
    return is_equal<T,L,C,K>(node->next[level],needle);
  }
  return false;
}

template <typename T,std::size_t L, typename C, typename K>
SkipListNode<T,L>*
remove_node(sp::SkipList<T, L, C> &list, const K &needle) noexcept {
  SkipListNode<T,L>* result = nullptr;
  SkipListNode<T,L>* current = nullptr;

  for(std::size_t level = L;level-- > 0;){
    if(!current){
      current = list.header[level];
      if(is_equal<T,L,C,K>(current,needle)){
        result = current;
        list.header[level] = current->next[level];
        //TODO done on this leve, go to next level
      }
    }

    /*
     * Search this level for the closest predecessor to needle
     */
    current = find_level_predecessor<T,L,C,K>(current,level,needle);
    if(current){
      auto *next = current->next[level];
      if(is_equal<T,L,C,K>(next, needle)){
        result = next;

        //unlink result
        current->next[level] = next->next[level];
      }
    }
  }

  return result;
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

    /*
     * Randomly generate what level self will be inserted into. The level also
     * includes lower levels aswell.
     */
    const std::size_t target_level = random_level(list.state,L);

    /*
     * Search horizontally and vertically for the predecessor to self->value
     * limit the result to target_level inclusive.
     */
    auto *start = find_predecessor(list,target_level,self->value);

    for(std::size_t level=target_level+1; level-- > 0;) {
      if(!start){
        /*
         * Start from the beginning of the level since there was NO higher
         * level express-lane
         */
        start = list.header[level];
      }

      /*
       * Search this level for the predecessor to self->value
       */
      auto*predecessor = find_level_predecessor<T,L,C>(start, level, self->value);
      if(predecessor){
        /*
         * We update chain on this level by inserting self:
         * predecessor->self->next
         */
        auto next = predecessor->next[level];
        predecessor->next[level] = self;
        self->next[level] = next;

        /*
         * Start next level search from the predecessor node on this level
         */
        start = predecessor;
      }else {
        /*
         * This level is empty, or self is greater than the first node in chain:
         * thus inserting self first in level
         */
        auto *next =list.header[level];
        self->next[level]=next;
        list.header[level]= self;

        /*
         * Since we could not find a predecessor on this level we have to start
         * from the beginning on the next level
         */
        start = nullptr;
      }
    }//for

    return &self->value;
  }

  return nullptr;
}

template <typename T,std::size_t L, typename C, typename V>
std::tuple<T *, bool>
insert_unique(SkipList<T,L, C> &list, V &&val) noexcept {
  using namespace impl::SkipList;

  SkipListNode<T,L> *cache[L] = {nullptr};

  SkipListNode<T,L> *start = nullptr;
  for(std::size_t level=L; level-- > 0;) {
    if(!start){
      start = list.header[level];
    }

    start = cache[level] = find_level_predecessor<T,L,C>(start, level, val);
    if(cache[level]) {
      if(is_next_equal<T,L,C,V>(cache[level],val,level)){
        return std::make_tuple(&cache[level]->next[level]->value,false);
      }
    } else {
      if(is_equal<T,L,C,V>(list.header[level], val)){
        return std::make_tuple(&list.header[level]->value, false);
      }
    }
  }

  auto self = new (std::nothrow) SkipListNode<T,L>{std::forward<V>(val)};
  if(self){
    const std::size_t target_level = random_level(list.state,L);
    for(std::size_t level=target_level+1; level-- > 0;) {
      if(cache[level]){
        auto next = cache[level]->next[level];
        cache[level]->next[level] = self;
        self->next[level] = next;
      }else {
        auto *next =list.header[level];
        self->next[level]=next;
        list.header[level]= self;

        start = nullptr;
      }
    }
    return std::make_tuple(&self->value,true);
  }

  return std::make_tuple(nullptr,false);
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

template <typename T,std::size_t l, typename C,typename K>
bool
take(SkipList<T, l, C> &list,const K&needle, T &out) noexcept {
  using namespace impl::SkipList;

  auto *result = remove_node(list,needle);
  if(result){
    std::swap(result->value,out);//TODO not std

    delete result;
    return true;
  }

  return false;
}

template <typename T,std::size_t L, typename C, typename K>
bool
remove(SkipList<T, L, C> &list, const K &needle) noexcept {
  using namespace impl::SkipList;

  auto *result = remove_node(list,needle);
  if(result){
    delete result;
  }

  return result != nullptr;
}//sp::remove()

template <typename T,std::size_t L, typename C>
bool
is_empty(const SkipList<T, L, C> &list) noexcept {
  for(std::size_t i=0;i<L;++i){
    if(list.header[i]){
      return false;
    }
  }
  return true;
}

template <typename T,std::size_t l, typename C>
void swap(SkipList<T,l, C> &first,SkipList<T,l, C> &second)noexcept {
  for(std::size_t i=0;i<l;++i){
    std::swap(first.header[i], second.header[i]);
  }
  swap(first.state,second.state);
}

template <std::size_t L, typename C>
void
dump(const SkipList<int,L,C> &list) noexcept {
  using namespace impl::SkipList;

  if(list.header[0] == nullptr){
    return;
  }

  auto index_of = [&list](const auto *node) {
    const SkipListNode<int,L>*it = list.header[0];
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
    const SkipListNode<int,L>*it = list.header[0];
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

  for(std::size_t level=L;level-- >0;) {
    std::size_t print_index=0;
    const SkipListNode<int,L>*it = list.header[level];
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
}

}//namespace sp

#endif
