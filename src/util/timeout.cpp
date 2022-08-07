#include "timeout.h"
#include <ctime>
#include <limits>
#include <util/assert.h>

namespace sp {

Timestamp
now() noexcept {
  time_t t = std::time(nullptr);
  return Timestamp(sp::Seconds((std::uint64_t)t));
}

/* =========================================================================================
 * =========================================================================================
 * =========================================================================================
 */
/* Timestamp */
Timestamp::Timestamp(std::uint64_t v) noexcept
    : value(v) {
}

Timestamp::Timestamp(const Milliseconds &v) noexcept
    : value(v.value) {
}

Timestamp::Timestamp(const Seconds &v) noexcept
    : value(v.value * 1000) {
}

Timestamp::Timestamp(const Minutes &v) noexcept
    : Timestamp(Seconds(v)) {
}

Timestamp::Timestamp(const Hours &v) noexcept
    : Timestamp(Minutes(v)) {
}

Timestamp::operator int() const noexcept {
  assertx(value < std::numeric_limits<int>::max());
  // TODO maybe max < value ? max : value
  return int(value);
}

Timestamp::operator std::uint64_t() const noexcept {
  return value;
}

/* ===================================== */
// Timestamp
// Timestamp::operator+(const Timeout &o) const noexcept {
//   return Timestamp(value + o.value);
// }

Timestamp
Timestamp::operator+(const Milliseconds &o) const noexcept {
  return Timestamp(value + o.value);
}

Timestamp
Timestamp::operator+(const Seconds &v) const noexcept {
  Milliseconds o(v);
  return this->operator+(o);
}

Timestamp
Timestamp::operator+(const Minutes &v) const noexcept {
  Milliseconds o(v);
  return this->operator+(o);
}

Timestamp
Timestamp::operator+(const Hours &v) const noexcept {
  Milliseconds o(v);
  return this->operator+(o);
}
/* ===================================== */
Timestamp
Timestamp::operator-(const Timestamp &o) const noexcept {
  assertx(value >= o.value);
  return Timestamp(value - o.value);
}

// Timestamp
// Timestamp::operator-(const Timeout &o) const noexcept {
//   assertx(value >= o.value);
//   return Timestamp(value - o.value);
// }

Timestamp
Timestamp::operator-(const Milliseconds &o) const noexcept {
  assertx(value >= o.value);
  return Timestamp(value - o.value);
}

Timestamp
Timestamp::operator-(const Seconds &v) const noexcept {
  Timestamp o(v);
  return this->operator-(o);
}

Timestamp
Timestamp::operator-(const Minutes &v) const noexcept {
  Timestamp o(v);
  return this->operator-(o);
}

Timestamp
Timestamp::operator-(const Hours &v) const noexcept {
  Timestamp o(v);
  return this->operator-(o);
}

/* ===================================== */
bool
Timestamp::operator>(const Timestamp &o) const noexcept {
  return value > o.value;
}

bool
Timestamp::operator>(const Milliseconds &v) const noexcept {
  Timestamp o(v);
  return operator>(o);
}

bool
Timestamp::operator>(const Seconds &v) const noexcept {
  Timestamp o(v);
  return operator>(o);
}

bool
Timestamp::operator>(const Minutes &v) const noexcept {
  Timestamp o(v);
  return operator>(o);
}

bool
Timestamp::operator>(const Hours &v) const noexcept {
  Timestamp o(v);
  return operator>(o);
}

/* ===================================== */
bool
Timestamp::operator>=(const Timestamp &o) const noexcept {
  return value >= o.value;
}

bool
Timestamp::operator>=(const Milliseconds &o) const noexcept {
  return value >= o.value;
}

bool
Timestamp::operator>=(const Seconds &v) const noexcept {
  Timestamp o(v);
  return operator>=(o);
}

bool
Timestamp::operator>=(const Minutes &v) const noexcept {
  Timestamp o(v);
  return operator>=(o);
}

bool
Timestamp::operator>=(const Hours &v) const noexcept {
  Timestamp o(v);
  return operator>=(o);
}

/* ===================================== */
bool
Timestamp::operator<(const Timestamp &o) const noexcept {
  return value < o.value;
}

bool
Timestamp::operator<(const Milliseconds &o) const noexcept {
  return value < o.value;
}

bool
Timestamp::operator<(const Seconds &v) const noexcept {
  Timestamp o(v);
  return operator<(o);
}

bool
Timestamp::operator<(const Minutes &v) const noexcept {
  Timestamp o(v);
  return operator<(o);
}

bool
Timestamp::operator<(const Hours &v) const noexcept {
  Timestamp o(v);
  return operator<(o);
}

/* ===================================== */
bool
Timestamp::operator<=(const Timestamp &o) const noexcept {
  return value <= o.value;
}

bool
Timestamp::operator<=(const Milliseconds &o) const noexcept {
  return value <= o.value;
}

bool
Timestamp::operator<=(const Seconds &v) const noexcept {
  Timestamp o(v);
  return operator<=(o);
}

bool
Timestamp::operator<=(const Minutes &v) const noexcept {
  Timestamp o(v);
  return operator<=(o);
}

bool
Timestamp::operator<=(const Hours &v) const noexcept {
  Timestamp o(v);
  return operator<=(o);
}

/* ===================================== */
bool
Timestamp::operator==(const Timestamp &o) const noexcept {
  return value == o.value;
}

bool
Timestamp::operator==(const Milliseconds &o) const noexcept {
  return value == o.value;
}

bool
Timestamp::operator==(const Seconds &v) const noexcept {
  Timestamp o(v);
  return operator==(o);
}

bool
Timestamp::operator==(const Minutes &v) const noexcept {
  Timestamp o(v);
  return operator==(o);
}

bool
Timestamp::operator==(const Hours &v) const noexcept {
  Timestamp o(v);
  return operator==(o);
}

/* ===================================== */
bool
Timestamp::operator!=(const Timestamp &o) const noexcept {
  return value != o.value;
}

bool
Timestamp::operator!=(const Milliseconds &o) const noexcept {
  return value != o.value;
}

bool
Timestamp::operator!=(const Seconds &v) const noexcept {
  Timestamp o(v);
  return operator!=(o);
}

bool
Timestamp::operator!=(const Minutes &v) const noexcept {
  Timestamp o(v);
  return operator!=(o);
}

bool
Timestamp::operator!=(const Hours &v) const noexcept {
  Timestamp o(v);
  return operator!=(o);
}

/* ===================================== */

/* =========================================================================================
 * =========================================================================================
 * =========================================================================================
 */
#if 0
/* Timeout */
Timeout::Timeout(std::uint64_t v) noexcept
    : value(v) {
}

Timeout::Timeout(const Milliseconds &v) noexcept
    : value(v.value) {
}

Timeout::Timeout(const Seconds &v) noexcept
    : value(v.value * 1000) {
}

Timeout::Timeout(const Minutes &v) noexcept
    : Timeout(Seconds(v)) {
}

Timeout::Timeout(const Hours &v) noexcept
    : Timeout(Minutes(v)) {
}

Timeout::operator int() const noexcept {
  assertx(value < std::numeric_limits<int>::max());
  // TODO maybe max < value ? max : value
  return int(value);
}

Timeout::operator std::uint64_t() const noexcept {
  return value;
}

/* ===================================== */
Timeout
Timeout::operator+(const Timeout &o) const noexcept {
  return Timeout(value + o.value);
}

Timeout
Timeout::operator+(const Milliseconds &o) const noexcept {
  return Timeout(value + o.value);
}

Timeout
Timeout::operator+(const Seconds &v) const noexcept {
  Timeout o(v);
  return this->operator+(o);
}

Timeout
Timeout::operator+(const Minutes &v) const noexcept {
  Timeout o(v);
  return this->operator+(o);
}

Timeout
Timeout::operator+(const Hours &v) const noexcept {
  Timeout o(v);
  return this->operator+(o);
}
/* ===================================== */
Timeout
Timeout::operator-(const Timeout &o) const noexcept {
  assertx(value >= o.value);
  return Timeout(value - o.value);
}

Timeout
Timeout::operator-(const Milliseconds &o) const noexcept {
  assertx(value >= o.value);
  return Timeout(value - o.value);
}

Timeout
Timeout::operator-(const Seconds &v) const noexcept {
  Timeout o(v);
  return this->operator-(o);
}

Timeout
Timeout::operator-(const Minutes &v) const noexcept {
  Timeout o(v);
  return this->operator-(o);
}

Timeout
Timeout::operator-(const Hours &v) const noexcept {
  Timeout o(v);
  return this->operator-(o);
}

/* ===================================== */
bool
Timeout::operator>(const Timeout &o) const noexcept {
  return value > o.value;
}

bool
Timeout::operator>(const Milliseconds &v) const noexcept {
  Timeout o(v);
  return operator>(o);
}

bool
Timeout::operator>(const Seconds &v) const noexcept {
  Timeout o(v);
  return operator>(o);
}

bool
Timeout::operator>(const Minutes &v) const noexcept {
  Timeout o(v);
  return operator>(o);
}

bool
Timeout::operator>(const Hours &v) const noexcept {
  Timeout o(v);
  return operator>(o);
}

/* ===================================== */
bool
Timeout::operator>=(const Timeout &o) const noexcept {
  return value >= o.value;
}

bool
Timeout::operator>=(const Milliseconds &o) const noexcept {
  return value >= o.value;
}

bool
Timeout::operator>=(const Seconds &v) const noexcept {
  Timeout o(v);
  return operator>=(o);
}

bool
Timeout::operator>=(const Minutes &v) const noexcept {
  Timeout o(v);
  return operator>=(o);
}

bool
Timeout::operator>=(const Hours &v) const noexcept {
  Timeout o(v);
  return operator>=(o);
}

/* ===================================== */
bool
Timeout::operator<(const Timeout &o) const noexcept {
  return value < o.value;
}

bool
Timeout::operator<(const Milliseconds &o) const noexcept {
  return value < o.value;
}

bool
Timeout::operator<(const Seconds &v) const noexcept {
  Timeout o(v);
  return operator<(o);
}

bool
Timeout::operator<(const Minutes &v) const noexcept {
  Timeout o(v);
  return operator<(o);
}

bool
Timeout::operator<(const Hours &v) const noexcept {
  Timeout o(v);
  return operator<(o);
}

/* ===================================== */
bool
Timeout::operator<=(const Timeout &o) const noexcept {
  return value <= o.value;
}

bool
Timeout::operator<=(const Milliseconds &o) const noexcept {
  return value <= o.value;
}

bool
Timeout::operator<=(const Seconds &v) const noexcept {
  Timeout o(v);
  return operator<=(o);
}

bool
Timeout::operator<=(const Minutes &v) const noexcept {
  Timeout o(v);
  return operator<=(o);
}

bool
Timeout::operator<=(const Hours &v) const noexcept {
  Timeout o(v);
  return operator<=(o);
}

/* ===================================== */
bool
Timeout::operator==(const Timeout &o) const noexcept {
  return value == o.value;
}

bool
Timeout::operator==(const Milliseconds &o) const noexcept {
  return value == o.value;
}

bool
Timeout::operator==(const Seconds &v) const noexcept {
  Timeout o(v);
  return operator==(o);
}

bool
Timeout::operator==(const Minutes &v) const noexcept {
  Timeout o(v);
  return operator==(o);
}

bool
Timeout::operator==(const Hours &v) const noexcept {
  Timeout o(v);
  return operator==(o);
}

/* ===================================== */
bool
Timeout::operator!=(const Timeout &o) const noexcept {
  return value != o.value;
}

bool
Timeout::operator!=(const Milliseconds &o) const noexcept {
  return value != o.value;
}

bool
Timeout::operator!=(const Seconds &v) const noexcept {
  Timeout o(v);
  return operator!=(o);
}

bool
Timeout::operator!=(const Minutes &v) const noexcept {
  Timeout o(v);
  return operator!=(o);
}

bool
Timeout::operator!=(const Hours &v) const noexcept {
  Timeout o(v);
  return operator!=(o);
}
#endif
/* =========================================================================================
 * =========================================================================================
 * =========================================================================================
 */
/* Milliseconds */
Milliseconds::Milliseconds(std::uint64_t v) noexcept
    : value(v) {
}

Milliseconds::Milliseconds(const Timestamp &v) noexcept
    : value(v.value) {
}

Milliseconds::Milliseconds(const Seconds &v) noexcept
    : value(v.value * 1000) {
}

Milliseconds::Milliseconds(const Minutes &v) noexcept
    : Milliseconds(Seconds(v)) {
}

Milliseconds::Milliseconds(const Hours &v) noexcept
    : Milliseconds(Minutes(v)) {
}

Milliseconds::operator int() const noexcept {
  assertx(value < std::numeric_limits<int>::max());
  // TODO maybe max < value ? max : value
  return int(value);
}

Milliseconds::operator std::uint64_t() const noexcept {
  return value;
}

/* ===================================== */
Milliseconds
Milliseconds::operator+(const Milliseconds &o) const noexcept {
  return Milliseconds(value + o.value);
}

Milliseconds
Milliseconds::operator+(const Seconds &v) const noexcept {
  Milliseconds o(v);
  return this->operator+(o);
}

Milliseconds
Milliseconds::operator+(const Minutes &v) const noexcept {
  Milliseconds o(v);
  return this->operator+(o);
}

Milliseconds
Milliseconds::operator+(const Hours &v) const noexcept {
  Milliseconds o(v);
  return this->operator+(o);
}
/* ===================================== */
Milliseconds
Milliseconds::operator-(const Milliseconds &o) const noexcept {
  assertx(value >= o.value);
  return Milliseconds(value - o.value);
}

Milliseconds
Milliseconds::operator-(const Seconds &v) const noexcept {
  Milliseconds o(v);
  return this->operator-(o);
}

Milliseconds
Milliseconds::operator-(const Minutes &v) const noexcept {
  Milliseconds o(v);
  return this->operator-(o);
}

Milliseconds
Milliseconds::operator-(const Hours &v) const noexcept {
  Milliseconds o(v);
  return this->operator-(o);
}

/* ===================================== */
bool
Milliseconds::operator>(const Milliseconds &o) const noexcept {
  return value > o.value;
}

bool
Milliseconds::operator>(const Seconds &v) const noexcept {
  Milliseconds o(v);
  return operator>(o);
}

bool
Milliseconds::operator>(const Minutes &v) const noexcept {
  Milliseconds o(v);
  return operator>(o);
}

bool
Milliseconds::operator>(const Hours &v) const noexcept {
  Milliseconds o(v);
  return operator>(o);
}

/* ===================================== */
bool
Milliseconds::operator>=(const Milliseconds &o) const noexcept {
  return value >= o.value;
}

bool
Milliseconds::operator>=(const Seconds &v) const noexcept {
  Milliseconds o(v);
  return operator>=(o);
}

bool
Milliseconds::operator>=(const Minutes &v) const noexcept {
  Milliseconds o(v);
  return operator>=(o);
}

bool
Milliseconds::operator>=(const Hours &v) const noexcept {
  Milliseconds o(v);
  return operator>=(o);
}

/* ===================================== */
bool
Milliseconds::operator<(const Milliseconds &o) const noexcept {
  return value < o.value;
}

bool
Milliseconds::operator<(const Seconds &v) const noexcept {
  Milliseconds o(v);
  return operator<(o);
}

bool
Milliseconds::operator<(const Minutes &v) const noexcept {
  Milliseconds o(v);
  return operator<(o);
}

bool
Milliseconds::operator<(const Hours &v) const noexcept {
  Milliseconds o(v);
  return operator<(o);
}

/* ===================================== */
bool
Milliseconds::operator<=(const Milliseconds &o) const noexcept {
  return value <= o.value;
}

bool
Milliseconds::operator<=(const Seconds &v) const noexcept {
  Milliseconds o(v);
  return operator<=(o);
}

bool
Milliseconds::operator<=(const Minutes &v) const noexcept {
  Milliseconds o(v);
  return operator<=(o);
}

bool
Milliseconds::operator<=(const Hours &v) const noexcept {
  Milliseconds o(v);
  return operator<=(o);
}

/* ===================================== */
bool
Milliseconds::operator==(const Milliseconds &o) const noexcept {
  return value == o.value;
}

bool
Milliseconds::operator==(const Seconds &v) const noexcept {
  Milliseconds o(v);
  return operator==(o);
}

bool
Milliseconds::operator==(const Minutes &v) const noexcept {
  Milliseconds o(v);
  return operator==(o);
}

bool
Milliseconds::operator==(const Hours &v) const noexcept {
  Milliseconds o(v);
  return operator==(o);
}

/* ===================================== */
bool
Milliseconds::operator!=(const Milliseconds &o) const noexcept {
  return value != o.value;
}

bool
Milliseconds::operator!=(const Seconds &v) const noexcept {
  Milliseconds o(v);
  return operator!=(o);
}

bool
Milliseconds::operator!=(const Minutes &v) const noexcept {
  Milliseconds o(v);
  return operator!=(o);
}

bool
Milliseconds::operator!=(const Hours &v) const noexcept {
  Milliseconds o(v);
  return operator!=(o);
}
/*
 * =========================================================================================
 * =========================================================================================
 * =========================================================================================
 */
/* Seconds */
Seconds::Seconds(std::uint64_t v) noexcept
    : value(v) {
}

Seconds::Seconds(const Timestamp &v) noexcept
    : value(v.value / 1000) {
}

Seconds::Seconds(const Minutes &v) noexcept
    : value(v.value * 60) {
}

Seconds::Seconds(const Hours &v) noexcept
    : Seconds(Minutes(v)) {
}

/* ===================================== */
Seconds::operator time_t() const noexcept {
  assertx(value < std::numeric_limits<time_t>::max());
  return time_t(value);
}
/* ===================================== */

Seconds::Seconds(const Milliseconds &v, RoundMode m) noexcept
    : value(v.value / 1000) {
  if (m == RoundMode::UP) {
    if (v.value % 1000 != 0) {
      /* if not evenly divisable */
      ++value;
    }
  }
}
Seconds::Seconds(const Seconds &v, RoundMode) noexcept
    : Seconds(v) {
}

/* ===================================== */
Seconds
Seconds::operator+(const Seconds &o) const noexcept {
  return Seconds(value + o.value);
}

Seconds
Seconds::operator+(const Minutes &v) const noexcept {
  Seconds o(v);
  return Seconds(value + o.value);
}

Seconds
Seconds::operator+(const Hours &v) const noexcept {
  Seconds o(v);
  return Seconds(value + o.value);
}
/* ===================================== */
bool
Seconds::operator>(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self > o;
}

bool
Seconds::operator>(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self > o;
}

bool
Seconds::operator>(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self > o;
}

bool
Seconds::operator>(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self > o;
}

/* ===================================== */
bool
Seconds::operator>=(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self >= o;
}

bool
Seconds::operator>=(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self >= o;
}

bool
Seconds::operator>=(const Minutes &o) const noexcept {

  Milliseconds self(*this);
  return self >= o;
}

bool
Seconds::operator>=(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self >= o;
}

/* ===================================== */
bool
Seconds::operator<(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self < o;
}

bool
Seconds::operator<(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self < o;
}

bool
Seconds::operator<(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self < o;
}

bool
Seconds::operator<(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self < o;
}

/* ===================================== */
bool
Seconds::operator<=(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self <= o;
}

bool
Seconds::operator<=(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self <= o;
}

bool
Seconds::operator<=(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self <= o;
}

bool
Seconds::operator<=(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self <= o;
}

/* ===================================== */
bool
Seconds::operator==(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self == o;
}

bool
Seconds::operator==(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self == o;
}

bool
Seconds::operator==(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self == o;
}

bool
Seconds::operator==(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self == o;
}

/* ===================================== */
bool
Seconds::operator!=(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self != o;
}

bool
Seconds::operator!=(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self != o;
}

bool
Seconds::operator!=(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self != o;
}

bool
Seconds::operator!=(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self != o;
}
/*
 * =========================================================================================
 * =========================================================================================
 * =========================================================================================
 */

/* Minutes */
Minutes::Minutes(std::uint64_t v) noexcept
    : value(v) {
}

Minutes::Minutes(const Hours &v) noexcept
    : value(v.value * 60) {
}

Minutes
Minutes::operator+(const Minutes &o) const noexcept {
  return Minutes(value + o.value);
}

Minutes
Minutes::operator+(const Hours &v) const noexcept {
  Minutes o(v);
  return Minutes(value + o.value);
}
/* ===================================== */
bool
Minutes::operator>(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self > o;
}

bool
Minutes::operator>(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self > o;
}

bool
Minutes::operator>(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self > o;
}

bool
Minutes::operator>(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self > o;
}

/* ===================================== */
bool
Minutes::operator>=(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self >= o;
}

bool
Minutes::operator>=(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self >= o;
}

bool
Minutes::operator>=(const Minutes &o) const noexcept {

  Milliseconds self(*this);
  return self >= o;
}

bool
Minutes::operator>=(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self >= o;
}

/* ===================================== */
bool
Minutes::operator<(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self < o;
}

bool
Minutes::operator<(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self < o;
}

bool
Minutes::operator<(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self < o;
}

bool
Minutes::operator<(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self < o;
}

/* ===================================== */
bool
Minutes::operator<=(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self <= o;
}

bool
Minutes::operator<=(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self <= o;
}

bool
Minutes::operator<=(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self <= o;
}

bool
Minutes::operator<=(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self <= o;
}

/* ===================================== */
bool
Minutes::operator==(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self == o;
}

bool
Minutes::operator==(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self == o;
}

bool
Minutes::operator==(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self == o;
}

bool
Minutes::operator==(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self == o;
}

/* ===================================== */
bool
Minutes::operator!=(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self != o;
}

bool
Minutes::operator!=(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self != o;
}

bool
Minutes::operator!=(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self != o;
}

bool
Minutes::operator!=(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self != o;
}
/*
 * =========================================================================================
 * =========================================================================================
 * =========================================================================================
 */

/* Hours */
Hours::Hours(std::uint64_t v) noexcept
    : value(v) {
}

Hours
Hours::operator+(const Hours &o) const noexcept {
  return Hours(value + o.value);
}

/* ===================================== */
bool
Hours::operator>(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self > o;
}

bool
Hours::operator>(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self > o;
}

bool
Hours::operator>(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self > o;
}

bool
Hours::operator>(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self > o;
}

/* ===================================== */
bool
Hours::operator>=(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self >= o;
}

bool
Hours::operator>=(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self >= o;
}

bool
Hours::operator>=(const Minutes &o) const noexcept {

  Milliseconds self(*this);
  return self >= o;
}

bool
Hours::operator>=(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self >= o;
}

/* ===================================== */
bool
Hours::operator<(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self < o;
}

bool
Hours::operator<(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self < o;
}

bool
Hours::operator<(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self < o;
}

bool
Hours::operator<(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self < o;
}

/* ===================================== */
bool
Hours::operator<=(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self <= o;
}

bool
Hours::operator<=(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self <= o;
}

bool
Hours::operator<=(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self <= o;
}

bool
Hours::operator<=(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self <= o;
}

/* ===================================== */
bool
Hours::operator==(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self == o;
}

bool
Hours::operator==(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self == o;
}

bool
Hours::operator==(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self == o;
}

bool
Hours::operator==(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self == o;
}

/* ===================================== */
bool
Hours::operator!=(const Milliseconds &o) const noexcept {
  Milliseconds self(*this);
  return self != o;
}

bool
Hours::operator!=(const Seconds &o) const noexcept {
  Milliseconds self(*this);
  return self != o;
}

bool
Hours::operator!=(const Minutes &o) const noexcept {
  Milliseconds self(*this);
  return self != o;
}

bool
Hours::operator!=(const Hours &o) const noexcept {
  Milliseconds self(*this);
  return self != o;
}

} // namespace sp
