#include "knuth_morris_pratt_search.h"

#include <cstring>

namespace sp {
namespace kmp {
//=====================================
#if 0
algorithm kmp_search:
    input:
        an array of characters, S (the text to be searched)
        an array of characters, W (the word sought)
    output:
        an array of integers, P (positions in S at which W is found)
        an integer, nP (number of positions)

    define variables:
        an integer, j ← 0 (the position of the current character in S)
        an integer, k ← 0 (the position of the current character in W)
        an array of integers, T (the table, computed elsewhere)

    let nP ← 0

    while j < length(S) do
        if W[k] = S[j] then
            let j ← j + 1
            let k ← k + 1
            if k = length(W) then
                (occurrence found, if only first occurrence is needed, m ← j - k  may be returned here)
                let P[nP] ← j - k, nP ← nP + 1
                let k ← T[k] (T[length(W)] can't be -1)
        else
            let k ← T[k]
            if k < 0 then
                let j ← j + 1
                let k ← k + 1
#endif

static void
failure_function(char *T, const char *needle, std::size_t len) noexcept {
}

const char *
search(const char *text, std::size_t tlen, const char *needle,
       std::size_t nlen) noexcept {
  const char *txt_it = text;
  const char *const txt_end = txt_it + tlen;

  const char *n_it = needle;
  const char *const n_end = needle + nlen;

  char T[1];
  failure_function(T, needle, nlen);

  for (; txt_it != txt_end;) {
    if (*n_it == *txt_it) {
      ++n_it;
      ++txt_it;
      if (n_it == n_end) {
        return txt_it - nlen;
      }
    } else {
      break;
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

//=====================================
}
}
