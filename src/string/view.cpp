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

string_view::operator bool() const noexcept {
  return bool(str) && length > 0;
}

std::ostream &
operator<<(std::ostream &out, const string_view &str) noexcept {
  for (std::size_t i = 0; i < str.length; ++i) {
    out << str.str[i];
  }
  return out;
}

} // namespace sp
