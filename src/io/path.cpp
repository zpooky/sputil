#include "path.h"

#include <limits.h>
#include <cstring>

namespace fs {
/*
 * ts in linux/limits.h.
 * #define PATH_MAX        4096     # chars in a path name including nul
 */

Path::Path(const char *s) noexcept
    : str{0} {
  std::strcpy(str, s);
}
} // namespace fs
