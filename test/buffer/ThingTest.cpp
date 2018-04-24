#include "gtest/gtest.h"
#include <buffer/Thing.h>
#include <prng/xorshift.h>

TEST(ThingTest, test) {
  constexpr unsigned char max(128);
  sp::StaticCircularByteBuffer<256> buffer;
  sp::Thing thing(buffer, nullptr, nullptr);

  {
    unsigned char out = 255;
    ASSERT_EQ(peek_front(thing, out), std::size_t(0));
    ASSERT_EQ(std::size_t(out), std::size_t(255));
  }

  for (unsigned char i = 0; i < max; ++i) {
    ASSERT_TRUE(write(buffer, i));
  }
  ASSERT_EQ(max, length(buffer));

  {
    unsigned char out[512] = {0};
    auto pl = peek_front(thing, out);
    ASSERT_EQ(pl, std::size_t(max));
    for (std::size_t i = 0; i < pl; ++i) {
      ASSERT_EQ(out[i], (unsigned char)i);
    }
  }

  ASSERT_EQ(max, length(buffer));
  {
    ASSERT_EQ(std::size_t(0), marks(thing));
    sp::ThingMark m = mark(thing);
    m.rollback = true;

    ASSERT_EQ(std::size_t(1), marks(thing));
    ASSERT_EQ(max, length(buffer));

    {
      unsigned char out[512] = {0};
      auto pl = peek_front(thing, out);
      ASSERT_EQ(pl, std::size_t(max));
      for (std::size_t i = 0; i < pl; ++i) {
        ASSERT_EQ(out[i], (unsigned char)i);
      }
    }
    ASSERT_EQ(std::size_t(1), marks(thing));
    ASSERT_EQ(max, length(buffer));

    {
      unsigned char i = 0;
      for (; i < max / 2; ++i) {
        unsigned char out = 255;
        ASSERT_EQ(pop_front(thing, out), std::size_t(1));
        ASSERT_EQ(i, out);
      }
      {
        unsigned char out[512] = {0};
        auto pl = peek_front(thing, out);
        ASSERT_EQ(pl, std::size_t(max / 2));
        unsigned char cmp = i;
        for (std::size_t a = 0; a < pl; ++a) {
          ASSERT_EQ(out[a], cmp++);
        }
      }
      for (; i < max; ++i) {
        unsigned char out = 255;
        ASSERT_EQ(pop_front(thing, out), std::size_t(1));
        ASSERT_EQ(i, out);
      }
    }

    ASSERT_EQ(std::size_t(1), marks(thing));
    ASSERT_EQ(max, length(buffer));

    {
      unsigned char out = 255;
      ASSERT_EQ(peek_front(thing, out), std::size_t(0));
      ASSERT_EQ(std::size_t(out), std::size_t(255));
    }

    ASSERT_EQ(std::size_t(1), marks(thing));
    ASSERT_EQ(max, length(buffer));
  }
  ASSERT_EQ(std::size_t(0), marks(thing));

  //=============
  {
    {
      unsigned char out[512] = {0};
      auto pl = peek_front(thing, out);
      ASSERT_EQ(pl, std::size_t(max));
      for (std::size_t i = 0; i < pl; ++i) {
        ASSERT_EQ(out[i], (unsigned char)i);
      }
    }

    {
      unsigned char i = 0;
      for (; i < max / 2; ++i) {
        unsigned char out = 255;
        ASSERT_EQ(pop_front(thing, out), std::size_t(1));
        ASSERT_EQ(i, out);
      }
      {
        unsigned char out[512] = {0};
        auto pl = peek_front(thing, out);
        ASSERT_EQ(pl, std::size_t(max / 2));
        unsigned char cmp = i;
        for (std::size_t a = 0; a < pl; ++a) {
          ASSERT_EQ(out[a], cmp++);
        }
      }
      for (; i < max; ++i) {
        unsigned char out = 255;
        ASSERT_EQ(pop_front(thing, out), std::size_t(1));
        ASSERT_EQ(i, out);
      }
    }

    {
      unsigned char out = 255;
      ASSERT_EQ(std::size_t(0), peek_front(thing, out));
      ASSERT_EQ(std::size_t(out), std::size_t(255));
    }
  }
  //=============
}

TEST(ThingTest, test_autofill) {
  constexpr std::size_t cap = 128;
  unsigned char cnt = 0;
  sp::StaticCircularByteBuffer<cap> buffer;
  sp::Thing thing(buffer, &cnt, [](auto &b, void *arg) {
    /**/
    auto &i = *((unsigned char *)arg);
    while (write(b, i)) {
      ++i;
    }

    return true;
  });
  {
    unsigned char out[cap * 2];
    auto pl = peek_front(thing, out);
    ASSERT_EQ(cap, pl);
    for (unsigned char i = 0; i < pl; ++i) {
      ASSERT_EQ(i, out[i]);
    }
  }
  {
    unsigned char cmp = 0;
    for (std::size_t i = 0; i < 768; ++i) {
      unsigned char out = 255;
      ASSERT_EQ(1, pop_front(thing, out));
      ASSERT_EQ(cmp++, out);
    }
    for (std::size_t i = 0; i < 768; ++i) {
      constexpr std::size_t out_cap = 7;
      unsigned char out[out_cap];
      ASSERT_EQ(out_cap, pop_front(thing, out));
      for (std::size_t a = 0; a < out_cap; ++a) {
        ASSERT_EQ(cmp++, out[a]);
      }
    }
  }
}

TEST(ThingTest, test_mark) {
  constexpr std::size_t cap = 512;
  unsigned char cnt = 0;
  sp::StaticCircularByteBuffer<cap> buffer;
  sp::Thing thing(buffer, &cnt, [](auto &b, void *arg) {
    /**/
    auto &i = *((unsigned char *)arg);
    while (write(b, i)) {
      ++i;
    }

    return true;
  });

  {
    auto m = mark(thing);
    unsigned char cmp = 0;
    constexpr std::size_t out_cap = 7;

    for (std::size_t i = 0; i < cap; i += out_cap) {
      unsigned char out[out_cap];
      auto pl = pop_front(thing, out);
      // ASSERT_EQ(out_cap, pl);
      for (std::size_t a = 0; a < pl; ++a) {
        ASSERT_EQ(cmp++, out[a]);
      }
    }
  }
}

TEST(ThingTest, test_rand) {
  constexpr std::size_t cap = 333;
  unsigned char cnt = 0;
  sp::StaticCircularByteBuffer<512> buffer;
  sp::Thing thing(buffer, &cnt, [](auto &b, void *arg) {
    /**/
    auto &i = *((unsigned char *)arg);
    while (write(b, i)) {
      ++i;
    }

    return true;
  });
  unsigned char cmp = 0;

  prng::xorshift32 r(1);
  std::size_t it = 0;
  while (it++ < 10000) {
    // std::size_t t[] = {1, 0};
    // auto task = t[it % 2];
    const std::uint32_t task = uniform_dist(r, 0, 6);
    // const std::uint32_t task = 0;

    // printf("%zu,", task);
    if (task == 0) { // pop + mark-commit
      ASSERT_EQ(std::size_t(0), marks(thing));

      auto m = mark(thing);
      ASSERT_EQ(std::size_t(1), marks(thing));

      unsigned char out[cap] = {0};
      const auto l = uniform_dist(r, 1, cap);
      // auto l = 36;
      // printf("pop_front(thing, out, %zu)\n", l);
      auto pl = pop_front(thing, out, l);

      for (std::size_t i = 0; i < pl; ++i) {
        // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
        //        out[i] == cmp ? "true" : "false");
        ASSERT_EQ(out[i], cmp);
        cmp++;
      }

      m.rollback = false;
      // printf("---\n");
    } else if (task == 1) { // pop + mark-rollback
      ASSERT_EQ(std::size_t(0), marks(thing));

      auto m = mark(thing);
      ASSERT_EQ(std::size_t(1), marks(thing));

      unsigned char out[cap] = {0};
      auto pl = pop_front(thing, out, uniform_dist(r, 1, cap));

      unsigned char copy_cmp = cmp;
      for (std::size_t i = 0; i < pl; ++i) {
        // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], copy_cmp,
        //        out[i] == copy_cmp ? "true" : "false");
        ASSERT_EQ(out[i], copy_cmp++);
      }

      m.rollback = true;
      // printf("---\n");
    } else if (task == 2) { // pop
      ASSERT_EQ(std::size_t(0), marks(thing));

      unsigned char out[cap] = {0};
      auto pl = pop_front(thing, out, uniform_dist(r, 1, cap));

      for (std::size_t i = 0; i < pl; ++i) {
        ASSERT_EQ(out[i], cmp++);
      }

    } else if (task == 3) { // peek + mark-commit
      ASSERT_EQ(std::size_t(0), marks(thing));
      auto m = mark(thing);
      ASSERT_EQ(std::size_t(1), marks(thing));

      unsigned char out[cap] = {0};
      auto pl = peek_front(thing, out, uniform_dist(r, 1, cap));

      unsigned char copy_cmp = cmp;
      for (std::size_t i = 0; i < pl; ++i) {
        ASSERT_EQ(out[i], copy_cmp++);
      }

      m.rollback = true;
    } else if (task == 4) { // peek + mark-rollback
      ASSERT_EQ(std::size_t(0), marks(thing));
      auto m = mark(thing);
      ASSERT_EQ(std::size_t(1), marks(thing));

      unsigned char out[cap] = {0};
      auto pl = peek_front(thing, out, uniform_dist(r, 1, cap));

      unsigned char copy_cmp = cmp;
      for (std::size_t i = 0; i < pl; ++i) {
        ASSERT_EQ(out[i], copy_cmp++);
      }

      m.rollback = false;
    } else if (task == 5) { // peek
      ASSERT_EQ(std::size_t(0), marks(thing));

      unsigned char out[cap] = {0};
      auto pl = peek_front(thing, out, uniform_dist(r, 1, cap));

      unsigned char copy_cmp = cmp;
      for (std::size_t i = 0; i < pl; ++i) {
        ASSERT_EQ(out[i], copy_cmp++);
      }

    } else {
      printf("%zu: %u\n", it, task);
      assertx(false);
    }

    ASSERT_EQ(std::size_t(0), marks(thing));
    ASSERT_EQ(std::size_t(0), thing.read_head);
    ASSERT_EQ(std::size_t(0), thing.marked);
    {
      // unsigned char out = 255;
      // ASSERT_EQ(std::size_t(1), peek_front(thing, out));
      // ASSERT_EQ(cmp, out);
    }
  }
}

TEST(ThingTest, test_marked_nested) {
  constexpr std::size_t cap = 333;
  unsigned char cnt = 0;
  sp::StaticCircularByteBuffer<512> buffer;
  sp::Thing thing(buffer, &cnt, [](auto &b, void *arg) {
    /**/
    auto &i = *((unsigned char *)arg);
    while (write(b, i)) {
      ++i;
    }

    return true;
  });
  unsigned char cmp = 0;

  prng::xorshift32 r(1);
  std::size_t it = 0;
  while (it++ < 500) {
    /*commit-commit*/ {
      ASSERT_EQ(std::size_t(0), marks(thing));
      auto commit0 = mark(thing);
      commit0.rollback = false;
      ASSERT_EQ(std::size_t(1), marks(thing));

      /*commit*/ {
        unsigned char out[cap] = {0};
        const auto l = uniform_dist(r, 1, cap);
        // printf("pop_front(thing, out, %zu)\n", l);
        auto pl = pop_front(thing, out, l);

        for (std::size_t i = 0; i < pl; ++i) {
          // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
          //        out[i] == cmp ? "true" : "false");
          ASSERT_EQ(out[i], cmp++);
        }
      }

      /*commit*/ {
        ASSERT_EQ(std::size_t(1), marks(thing));
        auto commit1 = mark(thing);
        commit1.rollback = false;
        ASSERT_EQ(std::size_t(2), marks(thing));

        {
          unsigned char out[cap] = {0};
          const auto l = uniform_dist(r, 1, cap);
          // printf("pop_front(thing, out, %zu)\n", l);
          auto pl = pop_front(thing, out, l);

          for (std::size_t i = 0; i < pl; ++i) {
            // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
            //        out[i] == cmp ? "true" : "false");
            ASSERT_EQ(out[i], cmp++);
          }
        }
      }
    }
    //=======================
    {
      unsigned char out[cap] = {0};
      auto pl = pop_front(thing, out, uniform_dist(r, 1, cap));

      for (std::size_t i = 0; i < pl; ++i) {
        ASSERT_EQ(out[i], cmp++);
      }
    }
    ASSERT_EQ(std::size_t(0), marks(thing));
    //=======================
    /*commit-rollback*/ {
      ASSERT_EQ(std::size_t(0), marks(thing));
      auto commit0 = mark(thing);
      commit0.rollback = false;
      ASSERT_EQ(std::size_t(1), marks(thing));

      /*commit*/ {
        unsigned char out[cap] = {0};
        const auto l = uniform_dist(r, 1, cap);
        // printf("pop_front(thing, out, %zu)\n", l);
        auto pl = pop_front(thing, out, l);

        for (std::size_t i = 0; i < pl; ++i) {
          // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
          //        out[i] == cmp ? "true" : "false");
          ASSERT_EQ(out[i], cmp++);
        }
      }

      /*rollback*/ {
        ASSERT_EQ(std::size_t(1), marks(thing));
        auto commit1 = mark(thing);
        commit1.rollback = true;
        ASSERT_EQ(std::size_t(2), marks(thing));

        {
          unsigned char out[cap] = {0};
          const auto l = uniform_dist(r, 1, cap);
          // printf("pop_front(thing, out, %zu)\n", l);
          auto pl = pop_front(thing, out, l);

          unsigned char copy_cmp = cmp;
          for (std::size_t i = 0; i < pl; ++i) {
            // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
            //        out[i] == cmp ? "true" : "false");
            ASSERT_EQ(out[i], copy_cmp++);
          }
        }
      }
    }
    ASSERT_EQ(std::size_t(0), marks(thing));
    //=======================
    {
      unsigned char out[cap] = {0};
      auto pl = pop_front(thing, out, uniform_dist(r, 1, cap));

      for (std::size_t i = 0; i < pl; ++i) {
        ASSERT_EQ(out[i], cmp++);
      }
    }
    //=======================
    /*rollback-commit*/ {
      ASSERT_EQ(std::size_t(0), marks(thing));
      auto commit0 = mark(thing);
      commit0.rollback = true;
      ASSERT_EQ(std::size_t(1), marks(thing));

      unsigned char copy_cmp = cmp;
      /*commit*/ {
        unsigned char out[cap] = {0};
        const auto l = uniform_dist(r, 1, cap);
        // printf("pop_front(thing, out, %zu)\n", l);
        auto pl = pop_front(thing, out, l);

        for (std::size_t i = 0; i < pl; ++i) {
          // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
          //        out[i] == cmp ? "true" : "false");
          ASSERT_EQ(out[i], copy_cmp++);
        }
      }

      /*commit*/ {
        ASSERT_EQ(std::size_t(1), marks(thing));
        auto commit1 = mark(thing);
        commit1.rollback = false;
        ASSERT_EQ(std::size_t(2), marks(thing));

        {
          unsigned char out[cap] = {0};
          const auto l = uniform_dist(r, 1, cap);
          // printf("pop_front(thing, out, %zu)\n", l);
          auto pl = pop_front(thing, out, l);

          for (std::size_t i = 0; i < pl; ++i) {
            // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
            //        out[i] == cmp ? "true" : "false");
            ASSERT_EQ(out[i], copy_cmp++);
          }
        }
      }
    }
    ASSERT_EQ(std::size_t(0), marks(thing));
    //=======================
    {
      unsigned char out[cap] = {0};
      auto pl = pop_front(thing, out, uniform_dist(r, 1, cap));

      for (std::size_t i = 0; i < pl; ++i) {
        ASSERT_EQ(out[i], cmp++);
      }
    }
    //=======================
    /*rollback-rollback*/ {
      ASSERT_EQ(std::size_t(0), marks(thing));
      auto commit0 = mark(thing);
      commit0.rollback = true;
      ASSERT_EQ(std::size_t(1), marks(thing));

      unsigned char copy_cmp = cmp;
      /*commit*/ {
        unsigned char out[cap] = {0};
        const auto l = uniform_dist(r, 1, cap);
        // printf("pop_front(thing, out, %zu)\n", l);
        auto pl = pop_front(thing, out, l);

        for (std::size_t i = 0; i < pl; ++i) {
          // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
          //        out[i] == cmp ? "true" : "false");
          ASSERT_EQ(out[i], copy_cmp++);
        }
      }

      /*commit*/ {
        ASSERT_EQ(std::size_t(1), marks(thing));
        auto commit1 = mark(thing);
        commit1.rollback = true;
        ASSERT_EQ(std::size_t(2), marks(thing));

        {
          unsigned char out[cap] = {0};
          const auto l = uniform_dist(r, 1, cap);
          // printf("pop_front(thing, out, %zu)\n", l);
          auto pl = pop_front(thing, out, l);

          for (std::size_t i = 0; i < pl; ++i) {
            // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
            //        out[i] == cmp ? "true" : "false");
            ASSERT_EQ(out[i], copy_cmp++);
          }
        }
      }
    }
    ASSERT_EQ(std::size_t(0), marks(thing));
    //=======================
    {
      unsigned char out[cap] = {0};
      auto pl = pop_front(thing, out, uniform_dist(r, 1, cap));

      for (std::size_t i = 0; i < pl; ++i) {
        ASSERT_EQ(out[i], cmp++);
      }
    }
    //=======================
    /*commit-(rollback-commit)*/ {
      ASSERT_EQ(std::size_t(0), marks(thing));
      auto commit0 = mark(thing);
      commit0.rollback = false;
      ASSERT_EQ(std::size_t(1), marks(thing));

      /*commit*/ {
        unsigned char out[cap] = {0};
        const auto l = uniform_dist(r, 1, cap);
        // printf("pop_front(thing, out, %zu)\n", l);
        auto pl = pop_front(thing, out, l);

        for (std::size_t i = 0; i < pl; ++i) {
          // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
          //        out[i] == cmp ? "true" : "false");
          ASSERT_EQ(out[i], cmp++);
        }
      }

      /*rollback*/ {
        ASSERT_EQ(std::size_t(1), marks(thing));
        auto rollback1 = mark(thing);
        rollback1.rollback = true;
        ASSERT_EQ(std::size_t(2), marks(thing));

        {
          unsigned char out[cap] = {0};
          const auto l = uniform_dist(r, 1, cap);
          // printf("pop_front(thing, out, %zu)\n", l);
          auto pl = pop_front(thing, out, l);

          unsigned char copy_cmp = cmp;
          for (std::size_t i = 0; i < pl; ++i) {
            // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
            //        out[i] == cmp ? "true" : "false");
            ASSERT_EQ(out[i], copy_cmp++);
          }
        }
      }
      /*commit*/ {
        ASSERT_EQ(std::size_t(1), marks(thing));
        auto commit1 = mark(thing);
        commit1.rollback = false;
        ASSERT_EQ(std::size_t(2), marks(thing));

        /*commit*/ {
          unsigned char out[cap] = {0};
          const auto l = uniform_dist(r, 1, cap);
          // printf("pop_front(thing, out, %zu)\n", l);
          auto pl = pop_front(thing, out, l);

          for (std::size_t i = 0; i < pl; ++i) {
            // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
            //        out[i] == cmp ? "true" : "false");
            ASSERT_EQ(out[i], cmp++);
          }
        }
        /*commit*/ {
          ASSERT_EQ(std::size_t(2), marks(thing));
          auto commit2 = mark(thing);
          commit2.rollback = false;
          ASSERT_EQ(std::size_t(3), marks(thing));

          /*commit*/ {
            unsigned char out[cap] = {0};
            const auto l = uniform_dist(r, 1, cap);
            // printf("pop_front(thing, out, %zu)\n", l);
            auto pl = pop_front(thing, out, l);

            for (std::size_t i = 0; i < pl; ++i) {
              // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
              //        out[i] == cmp ? "true" : "false");
              ASSERT_EQ(out[i], cmp++);
            }
          }
          /*rollback*/ {
            ASSERT_EQ(std::size_t(3), marks(thing));
            auto rollback2 = mark(thing);
            rollback2.rollback = true;
            ASSERT_EQ(std::size_t(4), marks(thing));

            unsigned char copy_cmp = cmp;
            {
              unsigned char out[cap] = {0};
              const auto l = uniform_dist(r, 1, cap);
              // printf("pop_front(thing, out, %zu)\n", l);
              auto pl = pop_front(thing, out, l);

              for (std::size_t i = 0; i < pl; ++i) {
                // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
                //        out[i] == cmp ? "true" : "false");
                ASSERT_EQ(out[i], copy_cmp++);
              }
            }
            /*rollback*/ {
              ASSERT_EQ(std::size_t(4), marks(thing));
              auto rollback3 = mark(thing);
              rollback3.rollback = true;
              ASSERT_EQ(std::size_t(5), marks(thing));

              {
                unsigned char out[cap] = {0};
                const auto l = uniform_dist(r, 1, cap);
                // printf("pop_front(thing, out, %zu)\n", l);
                auto pl = pop_front(thing, out, l);

                for (std::size_t i = 0; i < pl; ++i) {
                  // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
                  //        out[i] == cmp ? "true" : "false");
                  ASSERT_EQ(out[i], copy_cmp++);
                }
              }
            }
          }
          /*rollback*/ {
            ASSERT_EQ(std::size_t(3), marks(thing));
            auto rollback2 = mark(thing);
            rollback2.rollback = true;
            ASSERT_EQ(std::size_t(4), marks(thing));

            {
              unsigned char out[cap] = {0};
              const auto l = uniform_dist(r, 1, cap);
              // printf("pop_front(thing, out, %zu)\n", l);
              auto pl = pop_front(thing, out, l);

              unsigned char copy_cmp = cmp;
              for (std::size_t i = 0; i < pl; ++i) {
                // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
                //        out[i] == cmp ? "true" : "false");
                ASSERT_EQ(out[i], copy_cmp++);
              }
            }
          }
        }
      }
    }
    ASSERT_EQ(std::size_t(0), marks(thing));
    //=======================
    {
      unsigned char out[cap] = {0};
      auto pl = pop_front(thing, out, uniform_dist(r, 1, cap));

      for (std::size_t i = 0; i < pl; ++i) {
        ASSERT_EQ(out[i], cmp++);
      }
    }
  }
}

static void
rec_pop(sp::Thing &thing, unsigned char &cmp, bool rollback,
        prng::xorshift32 &r, std::size_t lvl) {
  if (lvl == 10) {
    return;
  }

  constexpr std::size_t cap = 333;
  auto commit0 = mark(thing);
  commit0.rollback = rollback;
  {
    unsigned char out[cap] = {0};
    const auto l = uniform_dist(r, 1, cap);
    // printf("pop_front(thing, out, %zu)\n", l);
    auto pl = pop_front(thing, out, l);

    for (std::size_t i = 0; i < pl; ++i) {
      // printf("ASSERT_EQ(out[%d], cmp[%d]): %s\n", out[i], cmp,
      //        out[i] == cmp ? "true" : "false");
      ASSERT_EQ(out[i], cmp++);
    }
  }
  {
    auto l = uniform_dist(r, 0, 10);
    for (std::uint32_t i = 0; i < l; ++i) {
      auto rb = uniform_bool(r);
      if (rb) {
        unsigned char copy_cmp = cmp;
        rec_pop(thing, copy_cmp, rb, r, lvl + 1);
      } else {
        rec_pop(thing, cmp, rb, r, lvl + 1);
      }
    }
  }
}

TEST(ThingTest, test_marked_rand_nested) {
  unsigned char cnt = 0;
  sp::StaticCircularByteBuffer<512> buffer;
  sp::Thing thing(buffer, &cnt, [](auto &b, void *arg) {
    /**/
    auto &i = *((unsigned char *)arg);
    while (write(b, i)) {
      ++i;
    }

    return true;
  });
  unsigned char cmp = 0;

  prng::xorshift32 r(1);
  std::size_t it = 0;
  // while (it++ < 500) {
  rec_pop(thing, cmp, false, r, 0);
  // }
}
