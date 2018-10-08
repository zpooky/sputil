#include "knuth_morris_pratt_search.h"

#include <cstring>

namespace sp {
namespace kmp {
//=====================================
const char *
search(const char *text, std::size_t, const char *needle,
       std::size_t) noexcept {
  // TODO
  return nullptr;
}

const char *
search(const char *text, std::size_t tlen, const char *needle) noexcept {
  return search(text, tlen, needle, std::strlen(needle));
}

const char *
search(const char *text, const char *needle) noexcept {
  return search(text, std::strlen(text), needle);
}

//=====================================
}
}
