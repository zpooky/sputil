#ifndef SP_UTIL_IO_FD_H
#define SP_UTIL_IO_FD_H

namespace sp {

struct fd {
  int m_fd;

  explicit fd(int) noexcept;
  fd() noexcept;

  explicit operator bool() const noexcept;

  fd(const fd &) = delete;
  fd(fd &&o) noexcept;

  fd &
  operator=(const fd &) = delete;
  fd &
  operator=(const fd &&) = delete;

  ~fd() noexcept;

  explicit operator int() noexcept;

  explicit operator bool() noexcept;
};

void
swap(fd &, fd &) noexcept;

void
swap(fd &, fd &&) noexcept;

} // namespace sp

#endif
