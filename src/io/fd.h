#ifndef SP_UTIL_IO_FD_H
#define SP_UTIL_IO_FD_H

namespace sp {

struct fd {
  int m_fd;

  explicit fd(int) noexcept;
  fd() noexcept;

  fd(const fd &) = delete;
  fd(fd &&o) noexcept;

  fd &
  operator=(const fd &) = delete;
  fd &
  operator=(const fd &&) = delete;

  virtual ~fd() noexcept;

  explicit operator int() const noexcept;
  explicit operator bool() const noexcept;
};

void
swap(fd &, fd &) noexcept;

void
swap(fd &, fd &&) noexcept;

} // namespace sp

#endif
