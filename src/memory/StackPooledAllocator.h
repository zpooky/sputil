#ifndef SP_UTIL_MEMORY_STACK_POOLED_ALLOCATOR_H
#define SP_UTIL_MEMORY_STACK_POOLED_ALLOCATOR_H

#include <type_traits>
#include <cxxabi.h>

namespace sp {
template<typename T>
struct StackPooledAllocator {
  using value_type = T;
  using is_always_equal = std::false_type;
  using propagate_on_container_move_assignment = std::true_type;

  StackPooledAllocator() noexcept;
  ~StackPooledAllocator() noexcept {
    int status;
    char * demangled = abi::__cxa_demangle(typeid(*this).name(),0,0,&status);
    printf("%s->~dtor(%p)\n",demangled,this);
    free(demangled);
  };
  // template<typename U>
  // StackPooledAllocator(StackPooledAllocator<U>&&) noexcept {
  // }
  template<typename U>
    StackPooledAllocator(const StackPooledAllocator<U>&&) =delete;
  template<typename U>
    StackPooledAllocator(StackPooledAllocator<U>&o) noexcept {
      // static_assert(std::is_same<T,U>::value,"");
      int status;
      char * this_demangled = abi::__cxa_demangle(typeid(*this).name(),0,0,&status);
      char * o_demangled = abi::__cxa_demangle(typeid(o).name(),0,0,&status);
      printf("%s(%s)\n",this_demangled,o_demangled);
      free(this_demangled);
      free(o_demangled);
    }

  template<typename U>
    StackPooledAllocator<T>& operator=(const StackPooledAllocator<T>&) = delete;
  template<typename U>
    StackPooledAllocator<T>& operator=(const StackPooledAllocator<T>&&) = delete;

  T* allocate(std::size_t n) noexcept {
    printf("allocate(%zu, sizeof[%zu], alignof[%zu])\n",n, sizeof(T),alignof(T));
    void* mem = std::malloc(n*sizeof(T));
    if(mem){
      return static_cast<T*>(mem);
    }
    return nullptr;
  };

  void deallocate(T* p, std::size_t n) noexcept {
    printf("deallocate(%p,%zu)\n",p,n);
    std::free(p);
  }

  //Optional
  template<typename U, typename... Args>
    void construct(U * object, Args &&... args)
    {
      printf("construct(%p)\n",object);
      new (object) U(std::forward<Args>(args)...);
    }

  //Optional
  template<typename U>
    void destroy(U * object)
    {
      printf("destroy(%p)\n",object);
      object->~U();
    }

};

template <typename T, typename U>
bool operator==(const StackPooledAllocator<T>&, const StackPooledAllocator<U>&)noexcept;

template <typename T, typename U>
bool operator!=(const StackPooledAllocator<T>&, const StackPooledAllocator<U>&)noexcept;

/*
 * =======================================================
 */
template<typename T>
StackPooledAllocator<T>::StackPooledAllocator() noexcept {

    int status;
    char * demangled = abi::__cxa_demangle(typeid(this).name(),0,0,&status);
    printf("ctor[%s]\n",demangled);
    free(demangled);
  }

/*
 * template <typename T, typename U>
 * bool operator==(const StackPooledAllocator<T>&, const StackPooledAllocator<U>&)noexcept {
 *   return false;
 * }
 * 
 * template <typename T, typename U>
 * bool operator!=(const StackPooledAllocator<T>&, const StackPooledAllocator<U>&)noexcept {
 * return true;
 * }
 */

}

#endif
