#include "conversions.h"
#include "util/assert.h"
#include <climits>

namespace sp {
//=====================================
bool
parse_int(const char *str, const char *str_end, char &val) {
  long out = 0;
  if (!parse_int(str, str_end, out)) {
    return false;
  }

  if (out > CHAR_MAX) {
    return false;
  }

  if (out < CHAR_MIN) {
    return false;
  }

  val = (char)out;

  return true;
}

bool
parse_int(const char *str, const char *str_end, unsigned char &val) {
  long out = 0;
  if (!parse_int(str, str_end, out)) {
    return false;
  }

  if (out > UCHAR_MAX) {
    return false;
  }

  val = (unsigned char)out;

  return true;
}

//=====================================
bool
parse_int(const char *str, const char *str_end, short &val) {
  long out = 0;
  if (!parse_int(str, str_end, out)) {
    return false;
  }

  if (out > SHRT_MAX) {
    return false;
  }

  if (out < SHRT_MIN) {
    return false;
  }

  val = (short)out;

  return true;
}

bool
parse_int(const char *str, const char *str_end, unsigned short &val) {
  unsigned long out = 0;
  if (!parse_int(str, str_end, out)) {
    return false;
  }

  if (out > USHRT_MAX) {
    return false;
  }

  val = (unsigned short)out;

  return true;
}

//=====================================
bool
parse_int(const char *str, const char *str_end, int &val) {
  long out = 0;
  if (!parse_int(str, str_end, out)) {
    return false;
  }

  if (out > INT_MAX) {
    return false;
  }

  if (out < INT_MIN) {
    return false;
  }

  val = (int)out;

  return true;
}

bool
parse_int(const char *str, const char *str_end, unsigned int &val) {
  unsigned long out = 0;
  if (!parse_int(str, str_end, out)) {
    return false;
  }

  if (out > UINT_MAX) {
    return false;
  }

  val = (unsigned int)out;

  return true;
}

//=====================================
bool
parse_int(const char *str, const char *str_end, long &val) {
  char *end = nullptr;

  if (str >= str_end) {
    assertx(false);
    return false;
  }

  val = std::strtol(str, &end, 10);
  if (end != str_end) {
    return false;
  }

  if (errno == ERANGE) {
    return false;
  }

  return true;
}

bool
parse_int(const char *str, const char *str_end, unsigned long &val) {
  char *end = nullptr;

  if (str >= str_end) {
    assertx(false);
    return false;
  }

  val = std::strtoul(str, &end, 10);
  if (end != str_end) {
    return false;
  }

  if (errno == ERANGE) {
    return false;
  }

  return true;
}

//=====================================
bool
parse_int(const char *str, const char *str_end, long long &val) {
  char *end = nullptr;

  if (str >= str_end) {
    assertx(false);
    return false;
  }

  val = std::strtol(str, &end, 10);
  if (end != str_end) {
    return false;
  }

  if (errno == ERANGE) {
    return false;
  }

  return true;
}

bool
parse_int(const char *str, const char *str_end, unsigned long long &val) {
  char *end = nullptr;

  if (str >= str_end) {
    assertx(false);
    return false;
  }

  val = std::strtoull(str, &end, 10);
  if (end != str_end) {
    return false;
  }

  if (errno == ERANGE) {
    return false;
  }

  return true;
}

//=====================================
} // namespace sp
