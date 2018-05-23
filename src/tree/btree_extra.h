#ifndef SP_UTIL_TREE_BTREE_EXTRA_H
#define SP_UTIL_TREE_BTREE_EXTRA_H

#include <tree/btree.h>

namespace btree {
//=====================================

/*
 * template <typename T, typename C, typename K>
 * bool
 * remove(Tree<T, C> &, const K &) noexcept;
 */

template <std::size_t k, typename C>
void
dump(const Tree<int, k, C> &) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace impl {
namespace btree {
template <std::size_t keys>
std::size_t
dump_levels(BTNode<int, keys> *root, std::size_t acum = 0) noexcept {
  std::size_t result = acum;
  if (root) {
    acum += 1;
    result = acum;
    auto &c = root->children;
    for (std::size_t i = 0; i < length(c); ++i) {
      result = std::max(result, dump_levels(c[i], acum));
    }
  }
  return result;
}
std::size_t
dump_colums_for(std::size_t children, std::size_t level) {
  if (level == 0) {
    return 1;
  } else {
    return children * dump_colums_for(children, level - 1);
  }
}

void
dump_print_null_column() {
  printf("[XXXXX]");
}

template <typename T>
static void
dump_print_column(T *root) noexcept {
  printf("[");
  for (std::size_t i = 0; i < capacity(root->elements); ++i) {
    if (i != 0) {
      printf("|");
    }
    if (i < length(root->elements)) {
      printf("%02d", root->elements[i]);
    } else {
      printf("__");
    }
  }
  printf("]");
}

inline std::size_t
dump_max_node_children(std::size_t children, std::size_t level) {
  return dump_colums_for(children, level);
}

template <std::size_t keys>
inline void
dump_do_add_children(BTNode<int, keys> *root,
                     sp::StaticArray<BTNode<int, keys> *, 128> &result) {
  auto &children = root->children;
  std::size_t i = 0;
  for (; i < length(children); ++i) {
    auto res = insert(result, children[i]);
    assertx(res);
  }
  for (; i < capacity(children); ++i) {
    auto res = insert(result, nullptr);
    assertx(res);
  }
}

template <std::size_t keys>
void
dump_get_columns_for(BTNode<int, keys> *root, std::size_t level,
                     sp::StaticArray<BTNode<int, keys> *, 128> &result,
                     std::size_t orig_level) {

  if (root == nullptr) {
    std::size_t empties = dump_max_node_children(keys + 1, orig_level);
    for (std::size_t i = 0; i < empties; ++i) {
      auto res = insert(result, nullptr);
      assertx(res);
    }
  } else if (level == 0) {
    dump_do_add_children(root, result);
  } else {
    auto &children = root->children;
    std::size_t i = 0;
    for (; i < length(children); ++i) {
      dump_get_columns_for(children[i], level - 1, result, orig_level);
    }
    for (; i < capacity(children); ++i) {
      dump_get_columns_for<keys>(nullptr, std::size_t(level - 1), result,
                                 orig_level);
    }
  }
}
inline std::size_t
dump_calc_width(std::size_t elements, std::size_t max_elements) {
  // printf("elements[%zu] <= max_elements[%zu]\n", elements, max_elements);
  assertx(elements <= max_elements);
  return (max_elements / elements);
}

template <typename F>
void
dump_print_width(std::size_t width, F f) {
  std::size_t pre_post = (width - 1) / 2;

  for (std::size_t i = 0; i < pre_post; ++i) {
    printf("       ");
  }
  f();
  for (std::size_t i = 0; i < pre_post; ++i) {
    printf("       ");
  }
}

template <std::size_t keys>
void
dump_print(BTNode<int, keys> *root,
           sp::StaticArray<BTNode<int, keys> *, 128> &columns) {

  auto width = dump_calc_width(
      length(columns), dump_colums_for(keys + 1, dump_levels(root) - 1));
  for (std::size_t i = 0; i < length(columns); ++i) {
    dump_print_width(width, [&] {
      if (columns[i]) {
        dump_print_column(columns[i]);
      } else {
        dump_print_null_column();
      }
    });
  }
  printf("\n");
}

template <std::size_t keys>
void
dump_level(BTNode<int, keys> *root, std::size_t level) noexcept {

  sp::StaticArray<BTNode<int, keys> *, 128> columns;
  // insert(columns, root);
  dump_get_columns_for(root, level, columns, level);
  dump_print(root, columns);
}

template <std::size_t keys>
void
dump(BTNode<int, keys> *root) noexcept {
  if (root) {
    auto lvls = dump_levels(root);

    {
      sp::StaticArray<BTNode<int, keys> *, 128> columns;
      insert(columns, root);
      dump_print(root, columns);
    }
    for (std::size_t lvl = 0; lvl < lvls - 1; ++lvl) {
      dump_level(root, lvl);
    }
  }
  // // printf("levels[%zu]\n", lvls);
  // if (root) {
  //   print_print_column(root);
  //   // dump_children(root);
  //   for_each(root->children, [](auto c) { //
  //     dump(c);
  //   });
  // }
}

} // namespace btree
} // namespace impl

template <typename T>
static void
dump_children(T *root) noexcept {
  int height = 0;
  height = reduce(root->children, height, [](auto h, auto c) {
    if (c) {
      return h + 1;
    }
    return h;
  });
  int raw[T::no_keys] = {-1};
  for (std::size_t i = 0; i < length(root->children); ++i) {
    if (root->children[i]) {
      raw[i] = height--;
    }
  }
}

template <std::size_t k, typename C>
void
dump(const Tree<int, k, C> &tree) noexcept {
  using namespace btree::impl::btree;
  dump(tree.root);
}

//=====================================
}

#endif
