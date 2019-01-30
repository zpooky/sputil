#ifndef SP_UTIL_UTIL_CONVERSIONS_H
#define SP_UTIL_UTIL_CONVERSIONS_H

namespace sp {
//=====================================
bool
parse_int(const char *str, const char *str_end, char &val);

bool
parse_int(const char *str, const char *str_end, unsigned char &val);

//=====================================
bool
parse_int(const char *str, const char *str_end, short &val);

bool
parse_int(const char *str, const char *str_end, unsigned short &val);

//=====================================
bool
parse_int(const char *str, const char *str_end, int &val);

bool
parse_int(const char *str, const char *str_end, unsigned int &val);

//=====================================
bool
parse_int(const char *str, const char *str_end, long &val);

bool
parse_int(const char *str, const char *str_end, unsigned long &val);

//=====================================
bool
parse_int(const char *str, const char *str_end, long long &val);

bool
parse_int(const char *str, const char *str_end, unsigned long long &val);

//=====================================
}


#endif
