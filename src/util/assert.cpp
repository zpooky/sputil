#define BACKWARD_HAS_BFD 1
#include "assert.h"
#include <backward.hpp>
#include <csignal>
#include <exception>
// #include <cassert>

using namespace backward;

namespace sp {
namespace impl {

static void
print_header(FILE *os, unsigned thread_id) {
  fprintf(os, "Stack trace (most recent call last)");
  if (thread_id) {
    fprintf(os, " in thread %u:\n", thread_id);
  } else {
    fprintf(os, ":\n");
  }
}

static const char *
filename(const char *path, std::size_t len) noexcept {
  const char *it = path + len;
  while (it != path) {
    if (*it == '/') {
      ++it;
      break;
    }
    --it;
  }

  return it;
}

static void
print_source_loc(FILE *os, const char *indent,
                 const ResolvedTrace::SourceLoc &ln, void *addr = 0) {

  // fprintf(os, "%sSource \"%s\", line %i, in %s", indent,
  //         ln.filename.c_str(), (int)ln.line,
  //         ln.function.c_str());

  // make function name higlight
  fprintf(os, "%s"
              "%s"
              " \033[91m%s\033[0m"
              ":\033[92m%u\033[0m",
          indent,                                              //
          ln.function.c_str(),                                 //
          filename(ln.filename.c_str(), ln.filename.length()), //
          ln.line);

  if (false && addr != 0) {
    fprintf(os, " [%p]\n", addr);
  } else {
    fprintf(os, "\n");
  }
}

static void
print_trace(FILE *os, const ResolvedTrace &trace, Colorize &colorize) noexcept {
  fprintf(os, "#%-2u", trace.idx);
  bool already_indented = true;

  if (!trace.source.filename.size()) {
    // fprintf(os, "   Object \"%s\", at %p, in %s\n",
    // trace.object_filename.c_str(), trace.addr,
    //         trace.object_function.c_str());
    already_indented = false;

    fprintf(os, "   %s\n", trace.object_function.c_str());
  }

  for (size_t iidx = 0; iidx < trace.inliners.size(); ++iidx) {
    if (!already_indented) {
      fprintf(os, "   ");
    }
    const ResolvedTrace::SourceLoc &inliner_loc = trace.inliners[iidx];
    print_source_loc(os, " | ", inliner_loc);
    // if (snippet) {
    //   print_snippet(os, "    | ", inliner_loc, colorize, Color::purple,
    //                 inliner_context_size);
    // }
    already_indented = false;
  }

  if (trace.source.filename.size()) {
    if (!already_indented) {
      fprintf(os, "   ");
    }
    print_source_loc(os, "   ", trace.source, trace.addr);
    // if (snippet) {
    //   print_snippet(os, "      ", trace.source, colorize, Color::yellow,
    //                 trace_context_size);
    // }
  }
}

static void
print(FILE *os, StackTrace &st) noexcept {
  Colorize colorize(os);
  if (true) {
    colorize.init();
  }
  print_header(os, st.thread_id());

  TraceResolver _resolver;
  _resolver.load_stacktrace(st);

  for (std::size_t trace_idx = 0; trace_idx < st.size(); ++trace_idx) {
    auto trace = _resolver.resolve(st[trace_idx]);
    print_trace(os, trace, colorize);
  }
}

[[noreturn]] void
assert_func(const char *file, int line, const char * /*function prototype*/,
            const char *cond) noexcept {

  // assert(false);
  auto &dest = stdout;

  { // assert dump
    fprintf(dest, "assertion failed: (%s)\n", cond);
    fprintf(dest, "%s"
                  ": \033[92m%d\033[0m"
                  "\n\n",
            file, line);
  }

  { // backtrace
    StackTrace st;
    st.load_here(32);

    print(dest, st);
    // Printer p;
    //
    // // Even if there is a source location, also prints the object
    // // from where the trace came from.
    // p.object = false;
    //
    // // Print a little snippet of code if possible.
    // p.snippet = false;
    //
    // // Add the addresses of every source location to the trace.
    // p.address = false;
    //
    // p.print(st, dest);
  }

  { // gdb breakpoint
    std::raise(SIGINT);
    std::raise(SIGABRT);
    std::abort();
  }

  std::terminate();
}

} // namespace impl
} // namespace sp

namespace backward {

backward::SignalHandling sh;

} // namespace backward
