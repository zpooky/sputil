#include "djb2.h"

namespace djb2 {
//https://softwareengineering.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed

std::uint32_t
encode(const void *buf, std::size_t length, std::uint32_t hash) noexcept {
  const unsigned char *it = (const unsigned char*)buf;
  const unsigned char *end = it + length;

  for (;it != end;++it) {
    int c = *it;
    hash = ((hash << 5) + hash) + c;
  }

  return hash;
}

std::uint32_t
encode32(const void *buf, std::size_t length) noexcept {
  constexpr std::uint32_t hash=5381;
  return encode(buf,length,hash);
}

}

namespace djb2a {

std::uint32_t
encode(const void *buf, std::size_t length, std::uint32_t hash) noexcept {
  const unsigned char *it = (const unsigned char*)buf;
  const unsigned char *end = it + length;

  for (;it != end;++it) {
    int c = *it;
    hash = (33 * hash) ^ c;
  }

  return hash;
}

std::uint32_t
encode32(const void *buf, std::size_t length) noexcept {
  constexpr std::uint32_t hash=5381;
  return encode(buf,length,hash);
}

}
