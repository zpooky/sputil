#ifndef SP_UTIL_NP_LONGEST_PALINDROMIC_SUBSTRING_H
#define SP_UTIL_NP_LONGEST_PALINDROMIC_SUBSTRING_H

#include <string/view.h>

namespace np {
namespace rec {
sp::string_view
longest_palindromic_substring(const char *, std::size_t);

sp::string_view
longest_palindromic_substring(const char *);
}
}

#endif
