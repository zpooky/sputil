#include "file.h"

#include <buffer/BytesView.h>
#include <buffer/CircularByteBuffer.h>

#include <cstdio>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <sys/errno.h> //errno
#include <sys/stat.h>
#include <sys/uio.h> //writev
#include <unistd.h>

namespace fs {

static void
die(const char *s) {
  perror(s);
  std::terminate();
}

//-------------------+---------------
static sp::fd
int_open(const char *p, int flag, mode_t mode = 0) noexcept {
  // bool create = true;
  // if (create) {
  mode = S_IRUSR | S_IWUSR;
  // }

  int res = ::open(p, flag, mode);

  return sp::fd{res};
}

//-------------------+---------------
sp::fd
open_trunc(const char *p) noexcept {
  int flag = O_TRUNC | O_WRONLY | O_CREAT;
  return int_open(p, flag);
}
//-------------------+---------------

sp::fd
open_append(const char *p) noexcept {
  int flag = O_APPEND | O_WRONLY | O_CREAT;
  return int_open(p, flag);
}
//-------------------+---------------

sp::fd
open_read(const char *p) noexcept {
  int flag = O_RDONLY;
  return int_open(p, flag);
}
//-------------------+---------------

std::size_t
write(sp::fd &f, const unsigned char *raw, std::size_t raw_len) noexcept {
  assertx(bool(f));

  std::size_t result = 0;
  ssize_t written = 0;
  do {
    if (raw_len == 0) {
      break;
    }

    written = ::write(int(f), raw, raw_len);
    if (written > 0) {
      result += written;
      assertx(std::size_t(written) <= raw_len);

      raw_len -= written;
      raw += written;
    }

  } while ((written < 0 && errno == EAGAIN) && raw_len > 0);

  if (written < 0) {
    die("write()");
  }

  return written;
}
//-------------------+---------------

/*
 * template specializations for write()
 */
template <>
bool
write(sp::fd &f, sp::BytesView &b) noexcept {
  // sp::bencode_print(buf);
  unsigned char *const raw = offset(b);
  const std::size_t raw_len = remaining_read(b);

  std::size_t written = write(f, raw, raw_len);
  b.pos += written;

  return true; // TODO?
}

template <>
bool
write(sp::fd &f, sp::CircularByteBuffer &b) noexcept {
  assertx(bool(f));
  using BA = typename sp::CircularByteBuffer::BufferArray;

  ssize_t written = 0;
  do {
    // constexpr std::sizarr
    int points = 0;
    ::iovec point[2];
    {
      BA arr;
      assertx(read_buffer(b, arr));
      for (; std::size_t(points) < length(arr); ++points) {
        auto current = arr[std::size_t(points)];
        point[std::size_t(points)].iov_base = std::get<0>(current);
        point[std::size_t(points)].iov_len = std::get<1>(current);
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
write<sp::BytesView>(sp::fd &, sp::BytesView &) noexcept;

template bool
write<sp::CircularByteBuffer>(sp::fd &, sp::CircularByteBuffer &) noexcept;
//-------------------+---------------

std::size_t
read(sp::fd &f, unsigned char *raw, std::size_t raw_len) noexcept {
  assertx(bool(f));

  std::size_t result = 0;
  ssize_t readed = 0;
  do {
    if (raw_len == 0) {
      break;
    }

    readed = ::read(int(f), raw, raw_len);

    if (readed > 0) {
      assertx(std::size_t(readed) <= raw_len);
      result += readed;
    }
  } while (readed < 0 && errno == EAGAIN);

  if (readed < 0) {
    die("write()");
  }

  return readed;
}

template <>
bool
read(sp::fd &f, sp::BytesView &b) noexcept {
  unsigned char *const raw = offset(b);
  const std::size_t raw_len = remaining_write(b);

  std::size_t readed = read(f, raw, raw_len);
  b.pos += readed;

  return true; // TODO?
}

template <>
bool
read(sp::fd &f, sp::CircularByteBuffer &b) noexcept {
  assertx(bool(f));

  using BA = typename sp::CircularByteBuffer::BufferArray;

  ssize_t readed = 0;
  do {
    int points = 0;
    ::iovec point[2];
    {
      BA arr;
      assertx(write_buffer(b, arr));
      for (; std::size_t(points) < length(arr); ++points) {
        auto current = arr[std::size_t(points)];
        point[std::size_t(points)].iov_base = std::get<0>(current);
        point[std::size_t(points)].iov_len = std::get<1>(current);
      }
    }

    if (points == 0) {
      break;
    }

    readed = ::readv(int(f), point, points);

    if (readed > 0) {
      produce_bytes(b, readed);
    }

  } while (readed < 0 && errno == EAGAIN);

  return true;
}

template bool
read<sp::BytesView>(sp::fd &, sp::BytesView &) noexcept;

template bool
read<sp::CircularByteBuffer>(sp::fd &, sp::CircularByteBuffer &) noexcept;

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

bool
is_file(const char *const path) noexcept {
  sp::fd handle{open(path, O_RDONLY | O_CLOEXEC)};
  if (!handle) {
    return false;
  }

  struct ::stat out;
  memset(&out, 0, sizeof(out));
  if (fstat(int(handle), &out) < 0) {
    return false;
  }

  if (S_ISREG(out.st_mode)) {
    return true;
  }

  return false;
}

// bool
// is_socket(const url::Path &) noexcept;

} // namespace fs
