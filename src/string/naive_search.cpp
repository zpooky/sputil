#include "naive_search.h"
#include <cstring>
#include <util/array.h>

namespace sp {
namespace naive {
//=====================================
const char *
search(const char *text, std::size_t tlen, const char *needle,
       std::size_t nlen) noexcept {
  const char *it = text;
  const char *const end = it + tlen;

  for (; it != end && sp::distance(it, end) >= nlen; ++it) {
    if (std::memcmp(it, needle, nlen) == 0) {
      return it;
    }
  }

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
}
}
