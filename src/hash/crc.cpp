#include "crc.h"
#include <cstddef>
#include <cstring>
#include <memory>
#include <util/assert.h>

// https://github.com/Michaelangel007/crc32
// http://www.ross.net/crc/download/crc_v3.txt

// https://www.lammertbies.nl/comm/info/crc-calculation.html
// http://crccalc.com/

using byte = unsigned char;
using crc32_t = std::uint32_t;

template <typename T>
static T
reverse(const T &d) noexcept {
  // TODO
  return d;
}

template <typename crc_t>
static crc_t
crc_reversed(const void *b, std::size_t length, crc_t polynomial) noexcept {
  // - reversed polynomial
  // - reversed the reversed bytes
  // - reversed the reversed final CRC value

  constexpr std::size_t bits = 8;
  constexpr std::size_t crc_bits = sizeof(crc_t) * bits;
  constexpr crc_t high_bit = crc_t(1) << crc_t(crc_bits - 1);

  const byte *it = (const byte *)b;
  crc_t crc = -1;

  while (length--) {
    constexpr std::size_t left_shift = (crc_bits - bits);
    const crc_t current = crc_t(reverse(*it++)) << left_shift;
    crc = crc ^ current;

    // While (more message bits)
    //    Begin
    //    Shift the register left by one bit, reading the next bit of the
    //       augmented message into register bit position 0.
    //    If (a 1 bit popped out of the register during step 3)
    //       Register = Register XOR Poly.
    //    End
    // The register now contains the remainder.
    for (std::size_t bit = 0; bit < bits; ++bit) {
      // check if the bit being shifted left is 1
      const bool popped = (crc & high_bit) == high_bit;
      crc = (crc << 1);

      if (popped) {
        crc = crc ^ polynomial;
      }
    }
  }

  return reverse(~crc);
}

template <typename crc_t>
static crc_t
crc(const void *b, std::size_t length, std::uint32_t polynomial) noexcept {
  // This version of the algorithm is the faster implementation in hardware,
  // implemented by shifting bits right-to-left using a barrel shifter. As
  // oposed to the reversed algorithm which is the faster software
  // implementation.

  constexpr std::size_t bits = 8;

  const byte *it = (const byte *)b;
  crc_t crc = ~crc_t(0);

  while (length--) {
    const crc_t current = crc_t(*it++);
    crc = crc ^ current;

    for (std::size_t bit = 0; bit < bits; ++bit) {
      const bool popped = crc & crc_t(1);
      crc = (crc >> 1);

      if (popped) {
        crc = crc ^ polynomial;
      }
    }
  }
  return ~crc;
}

namespace crc32c {
std::uint32_t
encode(const void *b, std::size_t len) noexcept {
  // return crc_reversed<crc32_t>(b, len, crc32_t(0x1EDC6F41));
  crc32_t p(0x82F63B78);
  return crc<crc32_t>(b, len, p);
}
}

namespace crc32 {

std::uint32_t
encode(const void *b, std::size_t len) noexcept {
  // return crc_reversed<crc32_t>(b, len, crc32_t(0x04C11DB7));
  crc32_t p(0xEDB88320);
  return crc<crc32_t>(b, len, p);
}
}
