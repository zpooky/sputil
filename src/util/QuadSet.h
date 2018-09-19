#ifndef SP_UTIL_UTIL_QUADSET_H
#define SP_UTIL_UTIL_QUADSET_H

#include <cstddef>

namespace sp {
//=====================================
struct QuadSet {
  /**/
};

//=====================================
char
test(const QuadSet &, std::size_t) noexcept;

//=====================================
char
set(QuadSet &, std::size_t, char) noexcept;
}

#endif
