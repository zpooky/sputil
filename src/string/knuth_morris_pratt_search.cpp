#include "knuth_morris_pratt_search.h"

#include <collection/Array.h>
#include <cstdint>
#include <cstring>
#include <util/assert.h>

namespace sp {
namespace kmp {
//=====================================
#if 0
algorithm kmp_table:
  input:
      an array of characters, W (the word to be analyzed)
      an array of integers, T (the table to be filled)
  output:
      nothing (but during operation, it populates the table)

  define variables:
      an integer, pos ← 1 (the current position we are computing in T)
      an integer, cnd ← 0 (the zero-based index in W of the next character of the current candidate substring)

  let T[0] ← -1

  while pos < length(W) do
      if W[pos] = W[cnd] then
          let T[pos] ← T[cnd], pos ← pos + 1, cnd ← cnd + 1
      else
          let T[pos] ← cnd

          let cnd ← T[cnd] (to increase performance)

          while cnd >= 0 and W[pos] <> W[cnd] do
              let cnd ← T[cnd]

          let pos ← pos + 1, cnd ← cnd + 1

  let T[pos] ← cnd (only need when all word occurrences searched)
#endif
static void
failure_function(const char *needle, int32_t *table,
                 const std::size_t len) noexcept {
  int32_t i = 0;
  int32_t j = 0;

  table[i++] = -1;

  while (i < len) {
    if (needle[i] == needle[j]) {
      table[i] = table[j];
    } else {
      table[i] = j;
      j = table[j];

      while (j >= 0 && needle[j] != needle[i]) {
        j = table[j];
      } // while
    }
    i++;
    j++;
  } // while

  table[i] = j;
}

// static void
// failure_function(const char *needle, int32_t *T,
//                  const std::size_t len) noexcept {
//   // const char *n_it = needle;
//   // const char *const n_end = needle + len;
//
//   // const char *T_it = T;
//   // const char *const T_end = T + len;
//   std::int32_t pos = 0;
//   std::int32_t cnd = 0;
//
//   T[pos++] = -1;
//   while (pos < len) {
//     if (needle[pos] == needle[cnd]) {
//       T[pos] = T[cnd];
//     } else {
//       T[pos] = cnd;
//
//       cnd = T[cnd];
//
//       while (cnd >= 0 && needle[pos] != needle[cnd]) {
//         cnd = T[cnd];
//       }
//     }
//     pos++;
//     cnd++;
//   }
//
//   T[pos] = cnd;
// }

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

  sp::DynamicArray<std::int32_t> table(nlen + 1);
  assertxs(capacity(table) == nlen + 1, length(table), capacity(table), nlen);

  failure_function(needle, table.data(), nlen);
  table.length = capacity(table);

#if 0
  printf("ff: ");
  for (std::size_t i = 0; i < nlen; ++i) {
    printf("%d, ", table[i]);
  }
  printf("\n");
#endif

  std::size_t t = 0;
  std::int32_t n = 0;

  while (t < tlen) {
    assertxs(t < tlen, t, tlen);
    assertxs(n < nlen, n, nlen);

    if (needle[n] == text[t]) {
      ++n;
      ++t;

      assertxs(n <= nlen, n, nlen, t);
      if (n == nlen) {

        assertxs(t >= n, t, n);
        return text + (t - n);
      }
    } else {
      n = table[n];
      if (n < 0) {
        ++t;
        ++n;
      }
    }
  } // while

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
