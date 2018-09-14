#ifndef SP_UTIL_UTIL_TIMER_H
#define SP_UTIL_UTIL_TIMER_H

#include <collection/Array.h>
#include <cstdio>
#include <time.h>
#include <util/assert.h>

namespace sp {
//=====================================
struct TimerContex {
  /**/
  sp::DynamicArray<clock_t> measurement;

  TimerContex() noexcept;
};

//=====================================
template <typename time_cb>
void
timer(TimerContex &ctx, time_cb cb) noexcept;

//=====================================
TimerContex
average(const TimerContex &) noexcept;

//=====================================
TimerContex
median(TimerContex &) noexcept;

//=====================================
void
print(const TimerContex &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename time_cb>
void
timer(TimerContex &ctx, time_cb cb) noexcept {
  clock_t before = clock();
  cb();
  clock_t after = clock();

  clock_t diff = after - before;
  clock_t msec = diff * 1000 / CLOCKS_PER_SEC;

  auto &m = ctx.measurement;
  assertxs_n(push(m, msec), length(m), capacity(m));
}
}

#endif
