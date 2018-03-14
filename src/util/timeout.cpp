#include "timeout.h"
#include <cassert>
#include <ctime>
#include <limits>

namespace sp {

Timestamp
now() noexcept {
  time_t t = std::time(nullptr);
  return Timestamp(sp::Seconds(t));
}

/* Milliseconds */
Milliseconds::Milliseconds(std::uint64_t v) noexcept
    : value(v) {
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

Milliseconds::operator std::uint64_t() const noexcept {
  return value;
}

Milliseconds::operator int() const noexcept {
  assert(std::numeric_limits<int>::max() > value);
  // TODO maybe max < value ? max : value
  return int(value);
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
  assert(value >= o.value);
  return Milliseconds(value - o.value);
}

Milliseconds
Milliseconds::operator-(const Seconds &v) const noexcept {
  Milliseconds o(v);
  return this->operator-(v);
}

Milliseconds
Milliseconds::operator-(const Minutes &v) const noexcept {
  Milliseconds o(v);
  return this->operator-(v);
}

Milliseconds
Milliseconds::operator-(const Hours &v) const noexcept {
  Milliseconds o(v);
  return this->operator-(v);
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

Seconds::Seconds(const Minutes &v) noexcept
    : value(v.value * 60) {
}

Seconds::Seconds(const Hours &v) noexcept
    : Seconds(Minutes(v)) {
}
/* ===================================== */

Seconds::operator std::uint64_t() const noexcept {
  return value;
}

Seconds::operator std::int64_t() const noexcept {
  assert(std::numeric_limits<std::int64_t>::max() > value);
  return std::int64_t(value);
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
