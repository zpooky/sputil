#include "concurrent/Barrier.h"
#include "tree/bst.h"
#include <collection/Array.h>
#include <map/HashSetTree.h>
#include <prng/xorshift.h>
#include <util/Bitset.h>

static void
test();

int
main() {
  sp::Barrier b(1);
  binary::Tree<int> s;

  test();
}

static void
test() {
  sp::HashSetTree<std::uint32_t> set;
  prng::xorshift32 r(1);
  std::size_t i = 0;
  constexpr std::uint32_t range = 1024;
  constexpr std::size_t bits(sizeof(std::uint64_t) * 8);
  assertx(range % bits == 0);
  std::size_t length = range / bits;

  auto raw = new uint64_t[length]{0};
  sp::Bitset bset(raw, length);

  for (; i < range; ++i) {
    for_each(bset, [&set](std::size_t idx, bool v) {
      const std::uint32_t in(idx);
      auto res = lookup(set, in);
      if (v) {
        assertx(res);
        assertx(*res == in);
      } else {
        assertx(!res);
      }
    });

    const std::uint32_t current = uniform_dist(r, 0, range);
    {
      const bool v = test(bset, current);
      auto res = insert(set, std::size_t(current));
      if (v) {
        assertx(res == nullptr);
      } else {
        assertx(res);
        assertx(*res == current);
        assertx(false == sp::set(bset, std::size_t(current), true));
      }
    }
  }
  assertx(sp::rec::verify(set));
  // dump(set.tree);

  delete[] raw;
}
