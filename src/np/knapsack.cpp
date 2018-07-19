#include "knapsack.h"
#include <util/Matrix.h>

namespace np {

item::item(std::size_t v, std::size_t w) noexcept
    : value(v)
    , weight(w)
    , next{nullptr} {
}

static std::size_t
max(std::size_t f, std::size_t s) noexcept {
  return f > s ? f : s;
}

std::size_t
knapsack(std::size_t capacity, item *const items) noexcept {
  std::size_t value = 0;
  struct item *priv = nullptr;
  struct item *it = items;

  while (it) {
    if (it->weight <= capacity) {
      std::size_t other = it->value;

      if (priv) {
        priv->next = it->next;
        other += knapsack(capacity - it->weight, items);
        priv->next = it;
      } else {
        other += knapsack(capacity - it->weight, items->next);
      }

      value = max(value, other);
    }

    priv = it;
    it = it->next;
  }

  return value;
}

namespace dp {
static std::size_t
length(const struct item *it) {
  std::size_t result = 0;
  while (it) {
    it = it->next;
    ++result;
  }

  return result;
}

std::size_t
knapsack(const std::size_t capacity, const struct item *const items) noexcept {
  std::size_t itm_cnt = length(items);
  sp::Matrix<ssize_t> table{itm_cnt + 1, capacity + 1};
  for_each(table, [](std::size_t, std::size_t, ssize_t &v) {
    /**/
    v = -1;
  });

  for (std::size_t x = 0; x <= capacity; x++) {
    table[0][x] = 0;
  }

  // print(table);

  // - first row is empty since no items are a valid option
  // - A row for each $item
  // - A column for each $capacity in knapsack
  // - Each column store the accumulated value
  for (std::size_t y = 1; y < table.height; ++y) {
    const auto &current = items[y - 1];

    for (std::size_t x = 0; x < table.width; ++x) {
      if (current.weight > x) {
        /* We cannot fit current in this slit the max is still previous
        max
         */
        table[y][x] = table[y - 1][x] /*previous in rank */;
      } else {
        table[y][x] = max(table[y - 1][x] /*previous in rank */,
                          table[y - 1][x - current.weight] + current.value);
      }

      if (table[y][x]) {
        // print(table);
      }
    }
  }

  return table[itm_cnt][capacity];
}
}
}
