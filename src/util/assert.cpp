#include "assert.h"
#include <backward.hpp>
#include <csignal>
#include <exception>

namespace sp {
namespace impl {

[[noreturn]] void
assert_func(const char *file, int line, const char *,
            const char *cond) noexcept {

  auto &dest = stdout;

  {// assert dump
    fprintf(dest, "assertion failed: %s\n", cond);
    fprintf(dest, "in %s:%d\n", file, line);
  }

  {//backtrace
    using namespace backward;
    StackTrace st;
    st.load_here(32);
    Printer p;

    // Even if there is a source location, also prints the object
    // from where the trace came from.
    p.object = true;

    // Print a little snippet of code if possible.
    // p.snippet = true;

    // Add the addresses of every source location to the trace.
    p.address = true;

    p.print(st, dest);
  }

  { // gdb breakpoint
    std::raise(SIGINT);
  }

  std::terminate();
}

} // namespace sp::impl
} // namespace sp
