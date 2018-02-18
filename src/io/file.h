#ifndef SP_MAINLINE_DHT_FILE_H
#define SP_MAINLINE_DHT_FILE_H

#include <buffer/ByteBuffer.h>
#include <io/fd.h>

namespace file {

struct Path {
  char str[256];
  explicit Path(const char *) noexcept;
};

Path
parent(const Path &) noexcept;

/*============*/
sp::fd
open_trunc(const char *) noexcept;

sp::fd
open_append(const char *) noexcept;

sp::fd
open_read(const char *) noexcept;

bool
write(sp::fd &, sp::ByteBuffer &) noexcept;

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
} // namespace file

#endif
