#include "URandom.h"
#include <io/fd.h>
#include <io/file.h>
#include <unistd.h>
#include <util/numeric.h>

namespace prng {
URandom::URandom() noexcept
    : mfd(-1) {
  sp::fd f = file::open_read("/dev/urandom");

  using sp::swap;
  swap(mfd, f.m_fd);
}

URandom::~URandom() noexcept {
  sp::fd raii(mfd);
  mfd = -1;
}

void
swap(URandom &f, URandom &s) noexcept {
  using sp::swap;
  swap(f.mfd, s.mfd);
}

std::uint32_t
random(URandom &r) noexcept {
  std::uint32_t res;
  ::read(r.mfd, &res, sizeof(res));
  return res;
}

} // namespace prng
