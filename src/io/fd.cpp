#include "file.h"
#include "util/assert.h"
#include "util/numeric.h"
#include <unistd.h> //close

namespace sp {

fd::fd(int p_fd) noexcept
    : m_fd(p_fd) {
}

fd::fd() noexcept
    : fd(-1) {
}

fd::operator bool() const noexcept {
  return m_fd >= 0;
}

fd::fd(fd &&o) noexcept
    : m_fd(o.m_fd) {
  o.m_fd = -1;
}

fd::~fd() noexcept {
  if (bool(*this)) {
    if (::close(m_fd) < 0) {
      assertx(false);
    }
    m_fd = -1;
  }
}

fd::operator int() noexcept {
  return m_fd;
}

fd::operator bool() noexcept {
  return m_fd > 0;
}

void
swap(fd &f, fd &s) noexcept {
  using sp::swap;
  swap(f.m_fd, s.m_fd);
}

void
swap(fd &f, fd &&s) noexcept {
  return swap(f, s);
}

} // namespace sp
