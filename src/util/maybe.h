#ifndef SP_UTIL_UTIL_MAYBE_H
#define SP_UTIL_UTIL_MAYBE_H

#include <type_traits>
#include <utility>

namespace sp {

template <typename T>
class maybe {
private:
  typename std::aligned_storage<sizeof(T), alignof(T)>::type data;
  bool present;

public:
  constexpr maybe() noexcept;

  template <typename I>
  explicit maybe(I &&d) //
      noexcept(noexcept(T{std::forward<I>(std::declval<I &>())}));

  maybe(maybe<T> &&o) //
      noexcept(noexcept(T{std::declval<T &&>()}));

  maybe(const maybe<T> &) //
      noexcept(noexcept(T{std::declval<T &>()}));

  // TODO inplace construction

  maybe &
  operator=(const maybe &) = delete;
  maybe &
  operator=(const maybe &&) = delete;

  template <typename I>
  maybe<T> &
  operator=(I &&) noexcept;

  void
  clear() noexcept;

  ~maybe() noexcept;

  explicit operator bool() const noexcept;

  const T &
  get() const &noexcept;

  T &
      get() &
      noexcept;

  T &&
      get() &&
      noexcept;

  const T &
  get_or(const T &def) const noexcept;

  T &
  get_or(T &def) noexcept;

  template <typename Out>
  Out
  get_or(Out &&def) noexcept;
};

template <typename T, typename F>
auto
map(maybe<T> &in, F f) -> maybe<decltype(f(in.get()))> {
  using Result = maybe<decltype(f(in.get()))>;
  if (bool(in)) {
    return Result{f(in.get())};
  }
  return Result{};
}

template <typename T, typename F>
auto
map(maybe<T> &&in, F f) -> maybe<decltype(f(std::move(in).get()))> {
  using Result = maybe<decltype(f(std::move(in).get()))>;
  if (bool(in)) {
    return Result{f(std::move(in).get())};
  }
  return Result{};
}

template <typename T, typename F>
auto
map(const maybe<T> &in, F f) -> maybe<decltype(f(in.get()))> {
  using Result = maybe<decltype(f(in.get()))>;
  if (bool(in)) {
    return Result{f(in.get())};
  }
  return Result{};
}

template <typename T>
constexpr maybe<T>::maybe() noexcept
    : data{}
    , present{false} {
}

// noexcept specifier with the noexcept() operator
// Reference collapsing rules:
// std::declval<T &&>()  -> T&&
// std::declval<T>()     -> T&&
// std::declval<T&>()    -> T&
template <typename T>
template <typename I>
maybe<T>::maybe(I &&d) //
    noexcept(noexcept(T{std::forward<I>(std::declval<I &>())}))
    : data{}
    , present(true) {
  ::new (&data) T{std::forward<I>(d)};
}

template <typename T>
maybe<T>::maybe(maybe<T> &&o) //
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

template <typename T>
maybe<T>::maybe(const maybe<T> &o) //
    noexcept(noexcept(T{std::declval<T &>()}))
    : data{}
    , present{o.present} {
  if (present) {
    T *ptr = reinterpret_cast<T *>(&data);
    const T *other = reinterpret_cast<const T *>(&o.data);
    ::new (ptr) T{*other};
  }
}

template <typename T>
void
maybe<T>::clear() noexcept {
  if (present) {
    present = false;
    T *ptr = reinterpret_cast<T *>(&data);
    ptr->~T();
  }
}

template <typename T>
template <typename I>
maybe<T> &
maybe<T>::operator=(I &&c) noexcept {
  clear();

  present = true;
  ::new (&data) T{std::forward<I>(c)};

  return *this;
}

template <typename T>
maybe<T>::~maybe() noexcept {
  clear();
}

template <typename T>
maybe<T>::operator bool() const noexcept {
  return present;
}

template <typename T>
const T &
maybe<T>::get() const &noexcept {
  const T *ptr = reinterpret_cast<const T *>(&data);
  return *ptr;
}

template <typename T>
    T &
    maybe<T>::get() &
    noexcept {
  T *ptr = reinterpret_cast<T *>(&data);
  return *ptr;
}

template <typename T>
    T &&
    maybe<T>::get() &&
    noexcept {
  T *ptr = reinterpret_cast<T *>(&data);
  return std::move(*ptr);
}

template <typename T>
const T &
maybe<T>::get_or(const T &def) const noexcept {
  if (present) {
    return get();
  }
  return def;
}

template <typename T>
T &
maybe<T>::get_or(T &def) noexcept {
  if (present) {
    return get();
  }
  return def;
}

template <typename T>
template <typename Out>
Out
maybe<T>::get_or(Out &&def) noexcept {
  if (present) {
    return static_cast<Out>(get());
  }
  return def;
}

} // namespace sp

#endif
