#include "tower_of_hanoi.h"
#include <collection/Array.h>
#include <cstddef>
#include <sort/util.h>
#include <string>
#include <util/assert.h>

namespace sp {

struct LL {
  DynamicStack<int> *from;
  DynamicStack<int> *to;
  DynamicStack<int> *aux;
};

//=====================================
namespace p {
static std::size_t
max_disk(DynamicStack<int> &s) {
  std::size_t result = 0;
  for_each(s, [&result](const int &c) {
    /**/
    auto c_str = std::to_string(c);
    result = std::max(result, c_str.length());
  });
  return result;
}

static std::size_t
spaceing(std::size_t max_c, std::size_t lvl);

static void
print_disk(int c, std::size_t max_c, std::size_t lvl, std::size_t max_lvl,
           std::string &line) {
  std::string tmp;
  tmp.append("| ");
  for (std::size_t i = 0; i < lvl; ++i) {
    tmp.append(" ");
  }
  std::string format = "%0";
  format.append(std::to_string(max_c));
  format.append("d");

  char buffer[100] = {0};
  sprintf(buffer, format.c_str(), c);
  tmp.append(buffer);
  for (std::size_t i = 0; i < lvl; ++i) {
    tmp.append(" ");
  }
  tmp.append(" |");

  std::size_t sp = tmp.length();
  if (lvl < max_lvl) {
    sp = spaceing(max_c, max_lvl);
  }
  const std::size_t target = sp;
  sp -= tmp.length();
  sp /= 2;

  for (std::size_t i = 0; i < sp; ++i) {
    std::string tmp2 = tmp;
    tmp = " ";
    tmp.append(tmp2);
  }
  while (tmp.length() < target) {
    tmp.append(" ");
  }

  line.append(tmp);
}

static void
print_empty_disk(std::size_t max_c, std::size_t lvl, std::size_t max_lvl,
                 std::string &line) {
  std::string tmp;
  print_disk(0, max_c, lvl, max_lvl, tmp);
  for (std::size_t i = 0; i < tmp.length(); ++i) {
    tmp[i] = ' ';
  }
  tmp[tmp.length() / 2] = '|';
  // printf("tmp: |%s|\n", tmp.c_str());
  line.append(tmp);
}

static std::size_t
spaceing(std::size_t max_c, std::size_t lvl) {
  std::string tmp;
  print_empty_disk(max_c, lvl, lvl, tmp);
  return tmp.length();
}

static void
print_raw(const char *msg, std::size_t max_c, std::size_t lvl,
          std::size_t max_lvl, std::string &line) {
  std::size_t sps = spaceing(max_c, max_lvl);
  std::size_t msg_len = std::strlen(msg);
  std::size_t prefix = (sps - msg_len) / 2;

  std::string result;
  for (std::size_t i = 0; i < prefix; ++i) {
    result.append(" ");
  }

  result.append(msg, msg_len);
  while (result.length() < sps) {
    result.append(" ");
  }
  line.append(result);
}

static void
print_stacks(DynamicStack<int> &from, DynamicStack<int> &to,
             DynamicStack<int> &aux) {

  const auto max_c =
      std::max(std::max(p::max_disk(from), p::max_disk(to)), p::max_disk(aux));
  const auto max_lvl =
      std::max(std::max(length(from), length(to)), length(aux));

  sp::DynamicArray<std::string> lines(max_lvl + 1);

  for (std::size_t i = 0; i < capacity(lines); ++i) {
    assertx_n(push(lines, ""));
  }
  {
    const char *titles[]{"source", "dest", "aux"};
    DynamicStack<int> *order[]{&from, &to, &aux};
    for (std::size_t stk = 0; stk < 3; ++stk) {
      auto &current = *order[stk];
      std::size_t lvl = 0;
      for (; lvl < max_lvl - length(current); ++lvl) {
        p::print_empty_disk(max_c, lvl, max_lvl, lines[lvl]);
      }
      for_each(current, [max_c, &lvl, max_lvl, &lines](const int &c) {
        p::print_disk(c, max_c, lvl, max_lvl, lines[lvl]);
        ++lvl;
      });
      print_raw(titles[stk], max_c, max_lvl, max_lvl, lines[lvl]);
    }
  }

  for (std::size_t i = 0; i < length(lines); ++i) {
    printf("%s\n", lines[i].c_str());
  }
  {
    std::size_t sps = spaceing(max_c, max_lvl) * 3;
    for (std::size_t i = 0; i < sps; ++i) {

      printf("-");
    }
    printf("\n\n");
  }
}

static void
print_stacks(LL &ll) {
  print_stacks(*ll.from, *ll.to, *ll.aux);
}
}

//=====================================
static void
tower_of_hanoi(LL &ll, std::size_t n, DynamicStack<int> &from,
               DynamicStack<int> &to, DynamicStack<int> &aux) noexcept {
  if (n > 0) {
    tower_of_hanoi(ll, n - 1, from, aux, to);

    int tmp = 0;
    assertx_n(pop(from, tmp));
    assertx_n(push(to, tmp));
    p::print_stacks(ll);

    tower_of_hanoi(ll, n - 1, aux, to, from);
  }
}

void
tower_of_hanoi(DynamicStack<int> &from, DynamicStack<int> &to) noexcept {
  assertx_f({
    const int *priv = peek(from);
    for_each(from, [&priv](const int &asd) {
      printf("%d, ", asd);
      /**/
      assertx(priv);
      assertxs(*priv <= asd, *priv, asd);
      priv = &asd;
    });
    printf("\n");
  });
  assertx(is_empty(to));

  DynamicStack<int> aux;

  LL ll;
  ll.from = &from;
  ll.to = &to;
  ll.aux = &aux;

  printf("___before___\n");
  p::print_stacks(ll);

  tower_of_hanoi(ll, length(from), from, to, aux);
}
}
