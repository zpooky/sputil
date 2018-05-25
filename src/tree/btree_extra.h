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

template <typename T>
inline void
fill_width(char);
// {
//   static_assert(false, "wrong");
// }

template <>
inline void
fill_width<char>(char filler) {
  printf("%c", filler);
}

template <>
inline void
fill_width<int>(char filler) {
  printf("%c%c", filler, filler);
}

template <>
inline void
fill_width<std::size_t>(char filler) {
  for (std::size_t i = 0; i < 5; ++i) {
    printf("%c", filler);
  }
}

template <std::size_t values, typename T>
void
print_null_column() {
  bool first = true;
  printf("[");
  for (std::size_t i = 0; i < values; ++i) {
    // printf("[XXXXX]");
    if (!first) {
      printf("%c", 'X');
    }
    fill_width<T>('X');
    first = false;
  }
  printf("]");
}

inline void
print_value(int v) {
  printf("%02d", v);
}

inline void
print_value(std::size_t v) {
  printf("%05zu", v);
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
      fill_width<typename Node::value_type>('_');
    }
  }
  printf("]");
}

template <std::size_t values, typename T>
void
print_background() {
  bool first = true;
  printf(" ");
  for (std::size_t i = 0; i < values; ++i) {
    if (!first) {
      printf("%c", ' ');
    }
    fill_width<T>(' ');
    first = false;
  }
  printf(" ");
}

#if 0
inline std::size_t
max_node_children(std::size_t children, std::size_t level) {
  if (level == 0) {
    return 1;
  }
  return children * max_node_children(children, level - 1);
}
#endif

template <typename Node, typename Result>
inline void
copy_all_children(Node *root, Result &result) {
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
columns_for_level(Node *root, std::size_t lvl, Result &out, std::size_t orig) {

  if (root == nullptr) {
    /* There is no $orig level in this branch, add all null children */
    for (std::size_t i = 0; i < Node::order; ++i) {
      // while (length(out) < empties) {
      auto res = insert(out, nullptr);
      assertx(res);
    }
    // assertxs(length(out) == empties, length(out), empties);
  } else if (lvl == 0) {
    /* we hit target level, add all children */
    copy_all_children(root, out);
  } else {
    auto &children = root->children;
    std::size_t i = 0;
    for (; i < length(children); ++i) {
      columns_for_level(children[i], lvl - 1, out, orig);
    }
    for (; i < capacity(children); ++i) {
      Node *tmp = nullptr;
      columns_for_level(tmp, std::size_t(lvl - 1), out, orig);
    }
  }
}

inline std::size_t
calc_width(std::size_t elements, std::size_t max_elements) {
  // printf("elements[%zu] <= max_elements[%zu]\n", elements, max_elements);
  assertxs(elements <= max_elements, elements, max_elements);
  return (max_elements / elements);
}

template <std::size_t values, typename T, typename F>
void
dump_print_width(std::size_t width, F f) {
  std::size_t pre_post = (width - 1) / 2;

  for (std::size_t i = 0; i < pre_post; ++i) {
    print_background<values, T>();
  }
  f();
  for (std::size_t i = 0; i < pre_post; ++i) {
    print_background<values, T>();
  }
}

template <typename Node, typename Result>
void
dump_print(Node *root, Result &columns) {

  auto col_cnt =
      column_count_for(capacity(root->children), level_cnt_for(root) - 1);
  auto width = calc_width(length(columns), col_cnt);
  for (std::size_t i = 0; i < length(columns); ++i) {
    auto f = [&] {
      if (columns[i]) {
        print_column(columns[i]);
      } else {
        print_null_column<Node::keys, typename Node::value_type>();
      }
    };
    dump_print_width<Node::keys, typename Node::value_type, decltype(f)>(width,
                                                                         f);
  }
  printf("\n");
}

template <typename Node>
void
dump_specific_level(Node *root, std::size_t level) noexcept {

  sp::StaticArray<Node *, 512> columns;
  // insert(columns, root);
  columns_for_level(root, level, /*OUT*/ columns, level);
  // printf("level:%zu\n", level);
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
