#ifndef SP_UTIL_NP_KNAPSACK_H
#define SP_UTIL_NP_KNAPSACK_H

#include <cstddef>

// https://en.wikipedia.org/wiki/Knapsack_problem
namespace np {

struct item {
  std::size_t value;
  std::size_t weight;
  struct item *next;

  item(std::size_t v, std::size_t w) noexcept;
};

std::size_t
knapsack(std::size_t capacity, item *) noexcept;

namespace dp {
// TODO document and understand
std::size_t
knapsack(std::size_t capacity, const struct item *) noexcept;
}

}

#endif
