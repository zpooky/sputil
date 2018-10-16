#include "rabin_karp_search.h"

#include <cstring>

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
struct RabinKarpHash {
  std::size_t length;
  const std::size_t capacity;
  std::uint32_t hash;
  char *const buffer;

  explicit RabinKarpHash(std::size_t c) noexcept;
};

static std::size_t
update(RabinKarpHash &, const char *begin, const char *end) noexcept {
  assertx(begin != end);
  std::size_t result = 0;

  while (begin != end) {
    // TODO
    ++result;

    if (length == capacity) {
      break;
    }
    ++length;
  }

  return result;
}

static std::uint32_t
hash(const RabinKarpHash &) noexcept {
  assertxs(length == capacity, length, capacity);
  return hash;
}

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
  assertxs(pattern, tlen, nlen);

  const char *t_it = text;
  const char *const t_end = text + tlen;

  RabinKarpHash t_hash(nlen);
  RabinKarpHash p_hash(nlen);
  {
    std::size_t u = update(p_hash, needle);
    assertxs(u == nlen, u, nlen);
  }

  while (t_it != t_end) {
    std::size_t u = update(t_hash, t_it, t_end);
    t_it += u;

    if (hash(t_hash) == hash(p_hash)) {

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
}
}
