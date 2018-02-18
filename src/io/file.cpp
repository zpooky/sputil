#include "file.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <sys/errno.h> //errno
#include <unistd.h>

namespace file {

static void
die(const char *s) {
  perror(s);
  std::terminate();
}

Path::Path(const char *s) noexcept
    : str{0} {
  std::strcpy(str, s);
}

static sp::fd
int_open(const char *p, int flag, mode_t mode = 0) noexcept {
  // bool create = true;
  // if (create) {
  flag |= O_CREAT;
  mode = S_IRUSR | S_IWUSR;
  // }

  int res = ::open(p, flag, mode);
  if (res  == -1) {
    die("open()");
  }

  return sp::fd{res};
}

sp::fd
open_trunc(const char *p) noexcept {
  int flag = O_TRUNC;
  return int_open(p, flag);
}

sp::fd
open_append(const char *p) noexcept {
  int flag = O_APPEND;
  return int_open(p, flag);
}

sp::fd
open_read(const char *p) noexcept {
  int flag = O_RDONLY;
  return int_open(p, flag);
}

bool
write(sp::fd &f, sp::ByteBuffer &b) noexcept {
  assert(int(f));

  ssize_t written = 0;
  do {
    // sp::bencode_print(buf);
    unsigned char *const raw = offset(b);
    const std::size_t raw_len = remaining_read(b);
    assert(raw_len > 0);

    written = ::write(int(f), raw, raw_len);
    if (written > 0) {
      b.pos += written;
    }
  } while ((written < 0 && errno == EAGAIN) && remaining_read(b) > 0);

  if (written < 0) {
    die("write()");
  }

  return true;
}

//
// bool
// append(sp::fd &, sp::ByteBuffer &) noexcept {
// }
/*============*/
// bool
// is_block_device(const url::Path &) noexcept {
//
// }
//
// bool
// is_character_device(const url::Path &) noexcept;
//
// bool
// is_directory(const url::Path &) noexcept;
//
// bool
// is_fifo(const url::Path &) noexcept;
//
// bool
// is_symlink(const url::Path &) noexcept;
//
// bool
// is_file(const url::Path &) noexcept;
//
// bool
// is_socket(const url::Path &) noexcept;

} // namespace file
