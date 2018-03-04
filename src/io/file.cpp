#include "file.h"

#include <buffer/BytesView.h>
#include <buffer/CircularByteBuffer.h>

#include <cassert>
#include <cstdio>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <sys/errno.h> //errno
#include <sys/uio.h>   //writev
#include <unistd.h>

namespace fs {

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
  mode = S_IRUSR | S_IWUSR;
  // }

  int res = ::open(p, flag, mode);
  if (res == -1) {
    die("open()");
  }

  return sp::fd{res};
}

sp::fd
open_trunc(const char *p) noexcept {
  int flag = O_TRUNC | O_WRONLY | O_CREAT;
  return int_open(p, flag);
}

sp::fd
open_append(const char *p) noexcept {
  int flag = O_APPEND | O_WRONLY | O_CREAT;
  return int_open(p, flag);
}

sp::fd
open_read(const char *p) noexcept {
  int flag = O_RDONLY;
  return int_open(p, flag);
}

/*
 * template specializations for write()
 */
template <>
bool
write(sp::fd &f, sp::BytesView &b) noexcept {
  assert(int(f));

  ssize_t written = 0;
  do {
    // sp::bencode_print(buf);
    unsigned char *const raw = offset(b);
    const std::size_t raw_len = remaining_read(b);
    if (raw_len == 0) {
      break;
    }

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

template <>
bool
write(sp::fd &f, sp::CircularByteBuffer &b) noexcept {
  using BA = typename sp::CircularByteBuffer::BufferArray;
  ssize_t written = 0;
  do {
    // constexpr std::sizarr
    std::size_t points = 0;
    ::iovec point[2];
    {
      BA arr;
      assert(read_buffer(b, arr));
      for (; points < length(arr); ++points) {
        auto current = arr[points];
        point[points].iov_base = std::get<0>(current);
        point[points].iov_len = std::get<1>(current);
      }
    }

    if (points == 0) {
      break;
    }

    written = ::writev(int(f), point, points);

    if (written > 0) {
      consume_bytes(b, written);
    }

  } while ((written < 0 && errno == EAGAIN) && remaining_read(b) > 0);

  return true;
}

/*
 * Explicit instantiation of these variations. Will make these variations
 * aviailiable during linking without requiring them to be placed in the header.
 */
template bool
write<sp::CircularByteBuffer>(sp::fd &, sp::CircularByteBuffer &) noexcept;

template bool
write<sp::BytesView>(sp::fd &, sp::BytesView &) noexcept;

//
// bool
// append(sp::fd &, sp::BytesView &) noexcept {
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

} // namespace fs
