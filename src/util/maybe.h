#ifndef SP_UTIL_UTIL_MAYBE_H
#define SP_UTIL_UTIL_MAYBE_H

#include <utility>
#include <type_traits>

namespace sp {

template <typename T>
class maybe {
private:
  typename std::aligned_storage<sizeof(T), alignof(T)>::type data;
  bool present;

public:
  constexpr maybe() noexcept
      : data{}
      , present{false} {
  }

  // noexcept specifier with the noexcept() operator
  // Reference collapsing rules:
  // std::declval<T &&>()  -> T&&
  // std::declval<T>()     -> T&&
  // std::declval<T&>()    -> T&
  template <typename I>
  explicit maybe(I &&d) //
      noexcept(noexcept(T{std::forward<I>(std::declval<I &>())}))
      : data{}
      , present(true) {
    ::new (&data) T{std::forward<I>(d)};
  }

  maybe(maybe<T> &&o) //
      noexcept(noexcept(T{std::declval<T &&>()}))
      : data{}
      , present{o.present} {
    if (present) {
      T *ptr = reinterpret_cast<T *>(&data);
      T *other = reinterpret_cast<T *>(&o.data);
      ::new (ptr) T{std::move(*other)};

      other->~T();
      o.present = false;
    }
  }
  // TODO inplace construction
  maybe(const maybe &) = delete;

  maybe &
  operator=(const maybe &) = delete;
  maybe &
  operator=(const maybe &&) = delete;

  ~maybe() noexcept {
    if (present) {
      present = false;
      T *ptr = reinterpret_cast<T *>(&data);
      ptr->~T();
    }
  }

  explicit operator bool() const noexcept {
    return present;
  }

  const T &
  get() const &noexcept {
    const T *ptr = reinterpret_cast<const T *>(&data);
    return *ptr;
  }

  T &
      get() &
      noexcept {
    T *ptr = reinterpret_cast<T *>(&data);
    return *ptr;
  }

  T &&
      get() &&
      noexcept {
    T *ptr = reinterpret_cast<T *>(&data);
    return std::move(*ptr);
  }

  const T &
  get_or(T &def) const noexcept {
    if (present) {
      return get();
    }
    return def;
  }

  T &
  get_or(T &def) noexcept {
    if (present) {
      return get();
    }
    return def;
  }

  template <typename Out>
  Out
  get_or(Out &&def) noexcept {
    if (present) {
      return static_cast<Out>(get());
    }
    return def;
  }
};

template<typename T, typename F>
auto map(maybe<T>&in,F f) -> maybe<decltype(f(in.get()))> {
  using Result = maybe<decltype(f(in.get()))>;
  if(bool(in)){
    return Result{f(in.get())};
  }
  return Result{};
}

template<typename T, typename F>
auto map(maybe<T>&&in,F f) -> maybe<decltype(f(std::move(in).get()))> {
  using Result = maybe<decltype(f(std::move(in).get()))>;
  if(bool(in)){
    return Result{f(std::move(in).get())};
  }
  return Result{};
}

template<typename T, typename F>
auto map(const maybe<T>&in,F f) -> maybe<decltype(f(in.get()))> {
  using Result = maybe<decltype(f(in.get()))>;
  if(bool(in)){
    return Result{f(in.get())};
  }
  return Result{};
}

}

#endif
