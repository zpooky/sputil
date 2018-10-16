#include "rabin_karp_search.h"
#include <cstdint>
#include <cstring>
#include <hash/RabinKarpHash.h>
#include <util/assert.h>

namespace sp {
namespace rk {
#if 0
function RabinKarp(string s[1..n], string pattern[1..m])
  hpattern := hash(pattern[1..m]);

  for i from 1 to n-m+1
    hs := hash(s[i..i+m-1])
    if hs = hpattern
      if s[i..i+m-1] = pattern[1..m]
        return i

  return not found
#endif
//=====================================

const char *
search(const char *const text, std::size_t tlen, //
       const char *const needle, std::size_t nlen) noexcept {
  if (nlen > tlen) {
    return nullptr;
  }
  if (tlen == 0) {
    return nullptr;
  }
  if (nlen == 0) {
    return nullptr;
  }

  assertxs(text, tlen, nlen);
  assertxs(needle, tlen, nlen);

  const char *t_it = text;
  const char *const t_end = text + tlen;

  RabinKarpHash t_hash(nlen);
  RabinKarpHash n_hash(nlen);
  {
    std::size_t u = update(n_hash, needle, nlen);
    assertxs(u == nlen, u, nlen);
  }

  while (t_it != t_end) {
    t_it = update(t_hash, t_it, t_end);

    if (hash(t_hash) == hash(n_hash)) {

      const char *const res = t_it - nlen;
      if (std::memcmp(res, needle, nlen) == 0) {
        return res;
      }
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
} // namespace rk
} // namespace sp
