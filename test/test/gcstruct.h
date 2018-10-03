#ifndef SP_TEST_UTIL_GC_STRUCT_H
#define SP_TEST_UTIL_GC_STRUCT_H

#include <cstddef>
#include <cstdint>
#include <hash/standard.h>
#include <util/assert.h>

namespace sp {
struct GcStruct {
  static std::int64_t active;
  static std::int64_t ctor;

  const std::size_t data;
  long alignment0;
  long alignment1;
  long alignment2;


  explicit GcStruct(std::size_t d)
      : data(d)
      , alignment0(active)
      , alignment1(active)
      , alignment2(active) {
    // printf("\nctor %p\n", this);
    ++active;
    ++ctor;
  }

  GcStruct(std::size_t d, int)
      : GcStruct(d) {
  }

  GcStruct(const GcStruct &) = delete;
  GcStruct(GcStruct &&o)
      : GcStruct(o.data, 0) {
  }

  GcStruct &
  operator=(const GcStruct &&) = delete;
  GcStruct &
  operator=(const GcStruct &) = delete;

  bool
  operator==(std::size_t o) const noexcept {
    return data == o;
  }

  // bool
  // operator==(int o) const noexcept {
  //   return data == o;
  // }

  bool
  operator==(const GcStruct &o) const noexcept {
    return this->operator==(o.data);
  }

  explicit operator std::size_t() const noexcept {
    return data;
  }

  ~GcStruct() {
    // printf("dtor %p\n", this);
    assertxs(active > 0, active);
    --active;
    assertxs(active >= 0, active);
  }
};

template <>
struct Hasher<GcStruct> {
  std::size_t
  operator()(std::size_t in) const {
    sp::Hasher<std::size_t> h;
    return h(in);
  }

  std::size_t
  operator()(const GcStruct &in) const {
    return operator()(in.data);
  }
};

template <>
struct Hasher<GcStruct *> {
  std::size_t
  operator()(std::size_t in) const {
    sp::Hasher<std::size_t> h;
    return h(in);
  }

  std::size_t
  operator()(const GcStruct *in) const {
    assertx(in);
    return operator()(in->data);
  }
};

template <>
struct Equality<GcStruct *> {
  bool
  operator()(std::size_t f, std::size_t s) const {
    return f == s;
  }

  bool
  operator()(const GcStruct *f, std::size_t s) const {
    assertx(f);
    return operator()(f->data, s);
  }

  bool
  operator()(const GcStruct *f, const GcStruct *s) const {
    assertx(f);
    assertx(s);
    return operator()(f->data, s->data);
  }
};

static void
swap(GcStruct &f, GcStruct &s) noexcept {
  GcStruct tmp(std::move(f));
  f.~GcStruct();
  new (&f) GcStruct(std::move(s));
  s.~GcStruct();
  new (&s) GcStruct(std::move(tmp));
}
}

#endif
