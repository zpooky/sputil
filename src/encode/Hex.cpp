#include "Hex.h"

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

  while (*it) {
    if (i > size) {
      return false;
    }
    // TODO verify in range 0-9 A-F a-f

    std::size_t index = static_cast<std::size_t>(*it++);
    std::uint8_t f = lookup[index];
    f = std::uint8_t(f << std::uint8_t(4));

    index = static_cast<std::size_t>(*it++);
    std::uint8_t s = lookup[index];
    dest[i++] = f | s;
  }

  return true;
} // namespace hex

bool
encode(const std::uint8_t *, /*OUT*/ char *,
       /*IN/OUT*/ std::size_t &) noexcept {
  // TODO
  return true;
}
} // namespace hex
