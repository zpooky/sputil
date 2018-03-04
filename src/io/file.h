#ifndef SP_MAINLINE_DHT_FILE_H
#define SP_MAINLINE_DHT_FILE_H

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

/*
 * sp::BytesView & sp::CircularByteBuffer are supported.
 */
template <typename Buffer>
bool
write(sp::fd &, Buffer &) noexcept;

/*============*/
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
