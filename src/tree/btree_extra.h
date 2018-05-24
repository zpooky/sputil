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
template <typename Node>
std::size_t
level_cnt_for(Node *root, std::size_t acum = 0) noexcept {
  std::size_t result = acum;
  if (root) {
    acum += 1;
    result = acum;

    auto &children = root->children;
    for (std::size_t i = 0; i < length(children); ++i) {
      result = std::max(result, level_cnt_for(children[i], acum));
    }
  }

  return result;
}

inline std::size_t
column_count_for(std::size_t children, std::size_t level) {
  if (level == 0) {
    return 1;
  } else {
    return children * column_count_for(children, level - 1);
  }
}

inline void
print_null_column() {
  printf("[XXXXX]");
}

inline void
print_value(int v) {
  printf("%02d", v);
}

inline void
print_value(std::size_t v) {
  printf("%02zu", v);
}

inline void
print_value(char v) {
  printf("%c", v);
}

template <typename Node>
static void
print_column(Node *root) noexcept {
  printf("[");
  for (std::size_t i = 0; i < capacity(root->elements); ++i) {
    if (i != 0) {
      printf("|");
    }
    if (i < length(root->elements)) {
      print_value(root->elements[i]);
    } else {
      printf("__");
    }
  }
  printf("]");
}

inline std::size_t
max_node_children(std::size_t children, std::size_t level) {
  if (level == 0) {
    return 1;
  }
  return children * max_node_children(children, level - 1);
}

template <typename Node, typename Result>
inline void
dump_do_add_children(Node *root, Result &result) {
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

template <typename Node, typename Result>
void
columns_for_level(Node *root, std::size_t level, Result &result,
                  std::size_t orig_level) {

  if (root == nullptr) {
    std::size_t empties = max_node_children(Node::order, orig_level);
    for (std::size_t i = 0; i < empties; ++i) {
      // while (length(result) < empties) {
      auto res = insert(result, nullptr);
      assertx(res);
    }
    // assertxs(length(result) == empties, length(result), empties);
  } else if (level == 0) {
    dump_do_add_children(root, result);
  } else {
    auto &children = root->children;
    std::size_t i = 0;
    for (; i < length(children); ++i) {
      columns_for_level(children[i], level - 1, result, orig_level);
    }
    for (; i < capacity(children); ++i) {
      Node *tmp = nullptr;
      columns_for_level(tmp, std::size_t(level - 1), result, orig_level);
    }
  }
}

inline std::size_t
calc_width(std::size_t elements, std::size_t max_elements) {
  // printf("elements[%zu] <= max_elements[%zu]\n", elements, max_elements);
  assertxs(elements <= max_elements, elements, max_elements);
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

template <typename Node, typename Result>
void
dump_print(Node *root, Result &columns) {

  auto col_cnt =
      column_count_for(capacity(root->children), level_cnt_for(root) - 1);
  auto width = calc_width(length(columns), col_cnt);
  for (std::size_t i = 0; i < length(columns); ++i) {
    dump_print_width(width, [&] {
      if (columns[i]) {
        print_column(columns[i]);
      } else {
        print_null_column();
      }
    });
  }
  printf("\n");
}

template <typename Node>
void
dump_specific_level(Node *root, std::size_t level) noexcept {

  sp::StaticArray<Node *, 512> columns;
  // insert(columns, root);
  columns_for_level(root, level, columns, level);
  dump_print(root, columns);
}

template <typename Node>
void
dump(Node *root) noexcept {
  if (root) {
    auto lvls = level_cnt_for(root);

    {
      sp::StaticArray<Node *, 512> columns;
      insert(columns, root);
      dump_print(root, columns);
    }
    for (std::size_t lvl = 0; lvl < lvls - 1; ++lvl) {
      dump_specific_level(root, lvl);
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

template <typename Node>
static void
dump_children(Node *root) noexcept {
  int height = 0;
  height = reduce(root->children, height, [](auto h, auto c) {
    if (c) {
      return h + 1;
    }
    return h;
  });
  int raw[Node::keys] = {-1};
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
