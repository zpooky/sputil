#include "stack.h"
#include <sys/resource.h>

namespace os {
bool
stack_atleast(std::size_t size) noexcept {
  struct rlimit rl;
  int result = getrlimit(RLIMIT_STACK, &rl);
  if (result == 0) {

    if (rl.rlim_cur < size) {
      rl.rlim_cur = size;
      result = setrlimit(RLIMIT_STACK, &rl);
      if (result != 0) {
        return false;
      }
    }

    return true;
  }

  return false;
}

} // namespace os
