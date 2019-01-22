#ifndef SP_UTIL_UTIL_TIMEOUT_H
#define SP_UTIL_UTIL_TIMEOUT_H

#include <util/round.h>

#include <cstddef>
#include <cstdint>
#include <time.h>

namespace sp {

class Milliseconds;
class Seconds;
class Minutes;
class Hours;

using Timestamp = Milliseconds;

Timestamp
now() noexcept;

class Milliseconds {
private:
public:
  std::uint64_t value;

  /* ===================================== */
  explicit Milliseconds(std::uint64_t v) noexcept;
  explicit Milliseconds(const Seconds &v) noexcept;
  explicit Milliseconds(const Minutes &v) noexcept;
  explicit Milliseconds(const Hours &v) noexcept;

  /* ===================================== */

  explicit operator int() const noexcept;
  explicit operator std::uint64_t() const noexcept;

  /* ===================================== */
  Milliseconds
  operator+(const Milliseconds &) const noexcept;
  Milliseconds
  operator+(const Seconds &) const noexcept;
  Milliseconds
  operator+(const Minutes &) const noexcept;
  Milliseconds
  operator+(const Hours &) const noexcept;

  /* ===================================== */
  Milliseconds
  operator-(const Milliseconds &) const noexcept;
  Milliseconds
  operator-(const Seconds &) const noexcept;
  Milliseconds
  operator-(const Minutes &) const noexcept;
  Milliseconds
  operator-(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator>(const Milliseconds &) const noexcept;
  bool
  operator>(const Seconds &) const noexcept;
  bool
  operator>(const Minutes &) const noexcept;
  bool
  operator>(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator>=(const Milliseconds &) const noexcept;
  bool
  operator>=(const Seconds &) const noexcept;
  bool
  operator>=(const Minutes &) const noexcept;
  bool
  operator>=(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator<(const Milliseconds &) const noexcept;
  bool
  operator<(const Seconds &) const noexcept;
  bool
  operator<(const Minutes &) const noexcept;
  bool
  operator<(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator<=(const Milliseconds &) const noexcept;
  bool
  operator<=(const Seconds &) const noexcept;
  bool
  operator<=(const Minutes &) const noexcept;
  bool
  operator<=(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator==(const Milliseconds &) const noexcept;
  bool
  operator==(const Seconds &) const noexcept;
  bool
  operator==(const Minutes &) const noexcept;
  bool
  operator==(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator!=(const Milliseconds &) const noexcept;
  bool
  operator!=(const Seconds &) const noexcept;
  bool
  operator!=(const Minutes &) const noexcept;
  bool
  operator!=(const Hours &) const noexcept;
};

class Seconds {
private:
public:
  std::uint64_t value;

  /* ===================================== */
  explicit Seconds(std::uint64_t v) noexcept;
  explicit Seconds(const Minutes &v) noexcept;
  explicit Seconds(const Hours &v) noexcept;

  /* ===================================== */
  explicit operator time_t() const noexcept;

  /* ===================================== */

  Seconds(const Milliseconds &v, RoundMode) noexcept;
  Seconds(const Seconds &v, RoundMode) noexcept;

  /* ===================================== */
  Seconds
  operator+(const Seconds &) const noexcept;
  Seconds
  operator+(const Minutes &) const noexcept;
  Seconds
  operator+(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator>(const Milliseconds &) const noexcept;
  bool
  operator>(const Seconds &) const noexcept;
  bool
  operator>(const Minutes &) const noexcept;
  bool
  operator>(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator>=(const Milliseconds &) const noexcept;
  bool
  operator>=(const Seconds &) const noexcept;
  bool
  operator>=(const Minutes &) const noexcept;
  bool
  operator>=(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator<(const Milliseconds &) const noexcept;
  bool
  operator<(const Seconds &) const noexcept;
  bool
  operator<(const Minutes &) const noexcept;
  bool
  operator<(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator<=(const Milliseconds &) const noexcept;
  bool
  operator<=(const Seconds &) const noexcept;
  bool
  operator<=(const Minutes &) const noexcept;
  bool
  operator<=(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator==(const Milliseconds &) const noexcept;
  bool
  operator==(const Seconds &) const noexcept;
  bool
  operator==(const Minutes &) const noexcept;
  bool
  operator==(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator!=(const Milliseconds &) const noexcept;
  bool
  operator!=(const Seconds &) const noexcept;
  bool
  operator!=(const Minutes &) const noexcept;
  bool
  operator!=(const Hours &) const noexcept;
};

class Minutes {
private:
public:
  std::uint64_t value;

  explicit Minutes(std::uint64_t v) noexcept;
  explicit Minutes(const Hours &v) noexcept;

  Minutes
  operator+(const Minutes &) const noexcept;
  Minutes
  operator+(const Hours &) const noexcept;
  /* ===================================== */
  bool
  operator>(const Milliseconds &) const noexcept;
  bool
  operator>(const Seconds &) const noexcept;
  bool
  operator>(const Minutes &) const noexcept;
  bool
  operator>(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator>=(const Milliseconds &) const noexcept;
  bool
  operator>=(const Seconds &) const noexcept;
  bool
  operator>=(const Minutes &) const noexcept;
  bool
  operator>=(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator<(const Milliseconds &) const noexcept;
  bool
  operator<(const Seconds &) const noexcept;
  bool
  operator<(const Minutes &) const noexcept;
  bool
  operator<(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator<=(const Milliseconds &) const noexcept;
  bool
  operator<=(const Seconds &) const noexcept;
  bool
  operator<=(const Minutes &) const noexcept;
  bool
  operator<=(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator==(const Milliseconds &) const noexcept;
  bool
  operator==(const Seconds &) const noexcept;
  bool
  operator==(const Minutes &) const noexcept;
  bool
  operator==(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator!=(const Milliseconds &) const noexcept;
  bool
  operator!=(const Seconds &) const noexcept;
  bool
  operator!=(const Minutes &) const noexcept;
  bool
  operator!=(const Hours &) const noexcept;
};

class Hours {
private:
public:
  std::uint64_t value;

  explicit Hours(std::uint64_t v) noexcept;

  Hours
  operator+(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator>(const Milliseconds &) const noexcept;
  bool
  operator>(const Seconds &) const noexcept;
  bool
  operator>(const Minutes &) const noexcept;
  bool
  operator>(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator>=(const Milliseconds &) const noexcept;
  bool
  operator>=(const Seconds &) const noexcept;
  bool
  operator>=(const Minutes &) const noexcept;
  bool
  operator>=(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator<(const Milliseconds &) const noexcept;
  bool
  operator<(const Seconds &) const noexcept;
  bool
  operator<(const Minutes &) const noexcept;
  bool
  operator<(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator<=(const Milliseconds &) const noexcept;
  bool
  operator<=(const Seconds &) const noexcept;
  bool
  operator<=(const Minutes &) const noexcept;
  bool
  operator<=(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator==(const Milliseconds &) const noexcept;
  bool
  operator==(const Seconds &) const noexcept;
  bool
  operator==(const Minutes &) const noexcept;
  bool
  operator==(const Hours &) const noexcept;

  /* ===================================== */
  bool
  operator!=(const Milliseconds &) const noexcept;
  bool
  operator!=(const Seconds &) const noexcept;
  bool
  operator!=(const Minutes &) const noexcept;
  bool
  operator!=(const Hours &) const noexcept;
};

} // namespace sp

#endif
