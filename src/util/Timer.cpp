#include "Timer.h"
#include <sort/introsort.h>

namespace sp {
//=====================================
TimerContext::TimerContext() noexcept
    : measurement(1024) {
}

//=====================================
TimerContext
average(const TimerContext &ctx) noexcept {
  clock_t avg = 0;
  for_each(ctx.measurement, [&avg](clock_t cur) {
    /**/
    avg += cur;
  });

  avg /= length(ctx.measurement);

  TimerContext result;
  assertx_n(push(result.measurement, avg));

  return result;
}

//=====================================
TimerContext
median(TimerContext &ctx) noexcept {
  auto &m = ctx.measurement;

  sp::rec::introsort(m.data(), length(m));

  std::size_t idx = length(m) / 2;

  TimerContext result;
  assertx_n(push(result.measurement, m[idx]));

  return result;
}

//=====================================
void
print(const TimerContext &ctx) noexcept {
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
