#ifndef SP_UTIL_OS_STACK_H
#define SP_UTIL_OS_STACK_H

#include <cstddef>

namespace os {
bool stack_atleast(std::size_t) noexcept;
}

#endif
