#include "Timer.h"
#include <sort/introsort.h>

namespace sp {
//=====================================
TimerContex::TimerContex() noexcept
    : measurement(1024) {
}

//=====================================
TimerContex
average(const TimerContex &ctx) noexcept {
  clock_t avg = 0;
  for_each(ctx.measurement, [&avg](clock_t cur) {
    /**/
    avg += cur;
  });

  avg /= length(ctx.measurement);

  TimerContex result;
  assertx_n(push(result.measurement, avg));

  return result;
}

//=====================================
TimerContex
median(TimerContex &ctx) noexcept {
  auto &m = ctx.measurement;

  sp::rec::introsort(m.data(), length(m));

  std::size_t idx = length(m) / 2;

  TimerContex result;
  assertx_n(push(result.measurement, m[idx]));

  return result;
}

//=====================================
void
print(const TimerContex &ctx) noexcept {
  for_each(ctx.measurement, [](clock_t msec) {
    clock_t sec = msec / 1000;
    clock_t rem_msec = msec % 1000;
    bool any_thing = false;
    if (sec > 0) {
      printf("%ld sec", sec);
      any_thing = true;
    }
    if (rem_msec > 0) {
      printf("%ld msec", rem_msec);
      any_thing = true;
    }
    if (!any_thing) {
      printf("0 msec");
    }
    printf("\n");
  });
}
}
