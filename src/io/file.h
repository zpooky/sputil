#ifndef SP_MAINLINE_DHT_FILE_H
#define SP_MAINLINE_DHT_FILE_H

#include <cstdint>
#include <io/fd.h>
#include <io/path.h>
#include <sys/types.h>

namespace fs {

//-------------------+---------------
struct DirectoryFd : public sp::fd {
  explicit DirectoryFd(int) noexcept;
  DirectoryFd() noexcept;

  virtual ~DirectoryFd();

  DirectoryFd(const DirectoryFd &) = delete;
  DirectoryFd(DirectoryFd &&o) noexcept;

  DirectoryFd &
  operator=(const DirectoryFd &) = delete;
  DirectoryFd &
  operator=(const DirectoryFd &&) = delete;
};

void
swap(DirectoryFd &, DirectoryFd &) noexcept;
void
swap(DirectoryFd &, DirectoryFd &&) noexcept;

/*============*/
sp::fd
open_trunc(DirectoryFd &parent, const char *fname) noexcept;

sp::fd
open_trunc(const char *path) noexcept;

sp::fd
open_append(const char *path) noexcept;

sp::fd
open_read(const char *path) noexcept;

sp::fd
open_read(DirectoryFd &parent, const char *fname) noexcept;

//-------------------+---------------
std::size_t
write(sp::fd &, const void *, std::size_t) noexcept;

template <std::size_t N>
std::size_t
write(sp::fd &f, const unsigned char (&buffer)[N]) noexcept {
  return write(f, buffer, N);
}

template <std::size_t N>
std::size_t
write(sp::fd &f, unsigned char (&buffer)[N]) noexcept {
  return write(f, buffer, N);
}

/*
 * sp::BytesView & sp::CircularByteBuffer are supported.
 */
template <typename Buffer>
bool
write(sp::fd &, Buffer &) noexcept;

//-------------------+---------------
std::size_t
read(sp::fd &, unsigned char *, std::size_t) noexcept;

template <std::size_t N>
std::size_t
read(sp::fd &f, unsigned char (&buffer)[N]) noexcept {
  return read(f, buffer, N);
}

/*
 * sp::BytesView & sp::CircularByteBuffer are supported.
 */
template <typename Buffer>
bool
read(sp::fd &, Buffer &) noexcept;

// template <std::size_t N>
// struct StaticBytesView;
//
// struct BytesView;
//
// // template <>
// template <std::size_t N>
// bool
// write(sp::fd &f, StaticBytesView<N> &b) noexcept {
//   BytesView &bv = b;
//   return write(f, bv);
// }
//-------------------+---------------

// bool
// is_block_device(const Path &) noexcept;
//
// bool
// is_character_device(const Path &) noexcept;
//
// bool
// is_directory(const Path &) noexcept;
//
// bool
// is_fifo(const Path &) noexcept;
//
// bool
// is_symlink(const Path &) noexcept;

bool
is_file(const char *) noexcept;

// bool
// is_socket(const Path &) noexcept;

//-------------------+---------------
DirectoryFd
open_dir(const char *path) noexcept;

bool
mkdirs(const char *path, mode_t) noexcept;

//-------------------+---------------
typedef bool (*for_each_files_cb_t)(DirectoryFd &, const char *fname, void *);

bool
for_each_files(DirectoryFd &dir, void *arg, for_each_files_cb_t);

bool
for_each_files(const char *path, void *arg, for_each_files_cb_t);

//-------------------+---------------
} // namespace fs

#endif
