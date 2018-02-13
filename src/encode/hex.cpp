#include "hex.h"
#include <cassert>

namespace hex {
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

bool
encode(const std::uint8_t *in, std::size_t in_length, /*OUT*/ char *out,
       /*IN/OUT*/ std::size_t &len) noexcept {
  const std::size_t size = len;
  len = 0;

  std::uint8_t lookup[16] = {0};
  {
    std::size_t i = 0;
    lookup[i++] = '0';
    lookup[i++] = '1';
    lookup[i++] = '2';
    lookup[i++] = '3';
    lookup[i++] = '4';
    lookup[i++] = '5';
    lookup[i++] = '6';
    lookup[i++] = '7';
    lookup[i++] = '8';
    lookup[i++] = '9';
    lookup[i++] = 'A';
    lookup[i++] = 'B';
    lookup[i++] = 'C';
    lookup[i++] = 'D';
    lookup[i++] = 'E';
    lookup[i++] = 'F';
  }
  for (std::size_t i = 0; i < in_length; ++i) {
    std::size_t f = (in[i] >> 4) & 0xf;
    assert(f < sizeof(lookup));

    std::size_t s = in[i] & 0xf;
    assert(s < sizeof(lookup));

    if (len >= size) {
      return false;
    }
    out[len++] = lookup[f];

    if (len >= size) {
      return false;
    }
    out[len++] = lookup[s];
  }

  return true;
}
} // namespace hex
