#ifndef SP_UTIL_STACK_STACK_H
#define SP_UTIL_STACK_STACK_H

#include <util/numeric.h>

namespace sp {
  template<typename T>
  struct Stack {
    T* buffer;
    std::size_t length;
    std::size_t capacity;

    Stack() noexcept;

    template<std::size_t SIZE>
    Stack(T(&)[SIZE]) noexcept;

    Stack(T*,std::size_t) noexcept;
  };

  template<typename T,typename V>
  T* push(Stack<T>&, V&&) noexcept;

  template<typename T>
  T* peek(Stack<T>&) noexcept;

  template<typename T>
  const T* peek(const Stack<T>&) noexcept;

  template<typename T>
  bool pop(Stack<T>&, T&out) noexcept;

/*
 * ==========================================================================
 */
  template<typename T>
  Stack<T>::Stack() noexcept
    : Stack(nullptr,0){
  }

  template<typename T>
  template<std::size_t SIZE>
  Stack<T>::Stack(T(&buffer)[SIZE]) noexcept 
    : Stack(buffer,SIZE) {
  }

  template<typename T>
  Stack<T>::Stack(T*b,std::size_t size) noexcept 
    : buffer{b}
    , length(0)
    , capacity(size) {
  }

  // XXX notexcept if T(val) is noexcept
  template<typename T,typename V>
  T* push(Stack<T>&stack, V&&val) noexcept {
    if(stack.length < stack.capacity){
      std::size_t idx = stack.length++;
      T *const raw = stack.buffer+idx;
      raw->~T();
      new (raw) T(std::forward<V>(val));

      return raw;
    }
    return nullptr;
  }

  template<typename T>
  T* peek(Stack<T>&stack) noexcept {
    const Stack<T>&c_stack = stack;
    return (T*)peek(c_stack);
  }

  template<typename T>
  const T* peek(const Stack<T>&stack) noexcept {
    if(stack.length > 0){
      return stack.buffer+(stack.length-1);
    }
    return nullptr;
  }

  //XXX noexcept if swap() is noexcept
  template<typename T>
  bool pop(Stack<T>&stack, T&out) noexcept {
    if(stack.length > 0){
      std::size_t idx = --stack.length;
      T& instack = stack.buffer[idx];
      using sp::swap;
      swap(instack,out);
      return true;
    }
    return false;
  }
}

#endif
