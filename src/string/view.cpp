#include "view.h"
#include <cstring>

namespace sp {

string_view::string_view(const char *s, std::size_t l) noexcept
    : str(s)
    , length(l) {
}

string_view::string_view(const char *s) noexcept
    : string_view(s, std::strlen(s)) {
}

bool
string_view::operator==(const char *o) const noexcept {
  auto l = std::strlen(o);
  return l == length && std::memcmp(str, o, l) == 0;
}

bool
string_view::operator==(const string_view &o) const noexcept {
  return o.length == length && std::memcmp(o.str, str, length) == 0;
}

} // namespace sp
