#ifndef SP_UTIL_TEST_OTHER_TMPL_H
#define SP_UTIL_TEST_OTHER_TMPL_H

#include <string>

//
// http://www.cplusplus.com/articles/1C75fSEw/

template <typename T>
struct TMPL {
  T *ptr;
  TMPL();
};

template <typename T>
T *
test_tmpl(TMPL<T> &t);

// typedef TMPL<int> TMPLint;
// typedef TMPL<double> TMPLdouble;

//=====================================
template <typename T>
class StringAdapter {
public:
  explicit StringAdapter(const T *);

  void
  doAdapterStuff();

private:
  std::basic_string<T> m_data;
};

// typedef StringAdapter<char> StrAdapter;
// typedef StringAdapter<wchar_t> WStrAdapter;
#endif
