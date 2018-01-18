#ifndef SP_UTIL_HEAP_BINARY_H
#define SP_UTIL_HEAP_BINARY_H

namespace heap {
template<typename T>
struct Binary {
  T* buffer;
  const std::size_t capacity;
  std::size_t length;
  Binary(T*,std::size_t) noexcept;
};


template<typename T,typename V>
T* insert(Binary<T>&,V&&) noexcept;

template<typename T,typename K>
bool remove(Binary<T>&,const K&) noexcept;

/*
 */
template<typename T>
Binary<T>::Binary(T*b,std::size_t c) noexcept 
: buffer{b},capacity{c} {

}

}


#endif
