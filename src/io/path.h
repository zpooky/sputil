#ifndef SP_MAINLINE_DHT_PATH_H
#define SP_MAINLINE_DHT_PATH_H

namespace fs {
struct Path {
  char str[256];
  explicit Path(const char *) noexcept;
};

Path
parent(const Path &) noexcept;
}

#endif
