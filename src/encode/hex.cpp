#include "hex.h"
#include <util/assert.h>

namespace hex {
//=====================================
bool
decode(const char *it, /*OUT*/ std::uint8_t *dest,
       /*IN/OUT*/ std::size_t &i) noexcept {
  const std::size_t size = i;
  i = 0;

  std::uint8_t lookup[('f' + 1) - '0'] = {0xff};
  lookup['0' - '0'] = 0x0;
  lookup['1' - '0'] = 0x1;
  lookup['2' - '0'] = 0x2;
  lookup['3' - '0'] = 0x3;
  lookup['4' - '0'] = 0x4;
  lookup['5' - '0'] = 0x5;
  lookup['6' - '0'] = 0x6;
  lookup['7' - '0'] = 0x7;
  lookup['8' - '0'] = 0x8;
  lookup['9' - '0'] = 0x9;
  //
  lookup['A' - '0'] = 0xA;
  lookup['B' - '0'] = 0xB;
  lookup['C' - '0'] = 0xC;
  lookup['D' - '0'] = 0xD;
  lookup['E' - '0'] = 0xE;
  lookup['F' - '0'] = 0xF;
  //
  lookup['a' - '0'] = 0xA;
  lookup['b' - '0'] = 0xB;
  lookup['c' - '0'] = 0xC;
  lookup['d' - '0'] = 0xD;
  lookup['e' - '0'] = 0xE;
  lookup['f' - '0'] = 0xF;

  while (*it != '\0') {
    if (i > size) {
      return false;
    }

    std::uint8_t f = 0;
    {
      std::size_t index = static_cast<std::size_t>(*it++) - '0';
      if (index >= sizeof(lookup)) {
        assertx(false);
        return false;
      }
      f = lookup[index];
      if (f == 0xff) {
        return false;
      }
      f = std::uint8_t(f << std::uint8_t(4));
    }

    if (*it == '\0') {
      return false;
    }

    std::uint8_t s = 0;
    {
      std::size_t index = static_cast<std::size_t>(*it++) - '0';
      if (index >= sizeof(lookup)) {
        assertx(false);
        return false;
      }
      s = lookup[index];
      if (s == 0xff) {
        return false;
      }
    }

    dest[i++] = f | s;
  }

  return true;
} // namespace hex

//=====================================
static const char enc_lookup[0xf + 1] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',

};

static void
encode_byte(std::uint8_t in, char &first, char &second) noexcept {
  std::size_t f = (in >> 4) & 0xf;
  assertxs(f < sizeof(enc_lookup), f);

  std::size_t s = in & 0xf;
  assertxs(s < sizeof(enc_lookup), s);

  first = enc_lookup[f];
  second = enc_lookup[s];
}

bool
encode(const std::uint8_t *in, std::size_t in_length, //
       /*OUT*/ char *out, /*IN/OUT*/ std::size_t &len) noexcept {
  const std::size_t out_size = len;
  len = 0;

  for (std::size_t i = 0; i < in_length; ++i) {

    char first = '\0';
    char second = '\0';
    encode_byte(in[i], /*OUT*/ first, /*OUT*/ second);

    if (len >= out_size) {
      return false;
    }
    out[len++] = first;

    if (len >= out_size) {
      return false;
    }
    out[len++] = second;
  } // for

  return true;
}

const std::uint8_t *
encode_inc(const std::uint8_t *const beg, const std::uint8_t *const end,
           /*OUT*/ char *out, /*IN/OUT*/ std::size_t &out_len) noexcept {
  const std::size_t out_size = out_len;
  const std::uint8_t *it = beg;

  out_len = 0;

  while (it != end) {
    std::size_t tmp_len = out_len;

    char first = '\0';
    char second = '\0';
    encode_byte(*it, /*OUT*/ first, /*OUT*/ second);

    if (tmp_len >= out_size) {
      break;
    }
    out[tmp_len++] = first;

    if (tmp_len >= out_size) {
      break;
    }
    out[tmp_len++] = second;

    out_len = tmp_len;
    ++it;
  }

  return it;
}

//=====================================
} // namespace hex
