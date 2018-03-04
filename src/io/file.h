#ifndef SP_MAINLINE_DHT_FILE_H
#define SP_MAINLINE_DHT_FILE_H

#include <cstdint>
#include <cstring>
#include <io/fd.h>
#include <io/path.h>

namespace fs {

/*============*/
sp::fd
open_trunc(const char *) noexcept;

sp::fd
open_append(const char *) noexcept;

sp::fd
open_read(const char *) noexcept;

//-------------------+---------------
std::size_t
write(sp::fd &, const unsigned char *, std::size_t) noexcept;

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

bool
is_block_device(const Path &) noexcept;

bool
is_character_device(const Path &) noexcept;

bool
is_directory(const Path &) noexcept;

bool
is_fifo(const Path &) noexcept;

bool
is_symlink(const Path &) noexcept;

bool
is_file(const Path &) noexcept;

bool
is_socket(const Path &) noexcept;
} // namespace fs

#endif
