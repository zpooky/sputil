#ifndef SP_UTIL_ENCODE_HEX_H
#define SP_UTIL_ENCODE_HEX_H

#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace hex {
//=====================================
/* $in is required to be zero terminated
 */
bool
decode(const char *in, /*OUT*/ std::uint8_t *,
       /*IN/OUT*/ std::size_t &) noexcept;

// template <std::size_t N>
// bool
// decode(const char *, /*OUT*/ std::uint8_t (&out)[N], /*IN/OUT*/ std::size_t
// &) noexcept;

//=====================================
bool
encode(const std::uint8_t *, std::size_t, /*OUT*/ char *,
       /*IN/OUT*/ std::size_t &) noexcept;

template <std::size_t S>
bool
encode(const std::uint8_t *in, std::size_t l, /*OUT*/ char (&out)[S]) noexcept {
  std::size_t out_len = S - 1;
  if (encode(in, l, out, out_len)) {
    out[out_len] = '\0';
    return true;
  }
  return false;
}

const std::uint8_t *
encode_inc(const std::uint8_t *beg, const std::uint8_t *end, /*OUT*/ char *,
           /*IN/OUT*/ std::size_t &) noexcept;

void
encode_print(const std::uint8_t *in, size_t length, FILE *fd) noexcept;

//=====================================
} // namespace hex
#endif
