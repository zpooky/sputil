#include "tmpl.h"
#include <string>

template <typename T>
TMPL<T>::TMPL()
    : ptr{nullptr} {
}

template <typename T>
T*
test_tmpl(TMPL<T> &tmpl) {
  return tmpl.ptr;
}

/*
 * explicit template function instantiation of test_tmpl<int> and test_tmpl<double>
 */
template int*
test_tmpl<int>(TMPL<int> &tmpl);

template double*
test_tmpl<double>(TMPL<double> &tmpl);


/*
 * explicit instantiation of struct TMPL<int> and TMPL<double>
 */
template struct TMPL<int>;
template struct TMPL<double>;
//=====================================
template <typename T>
StringAdapter<T>::StringAdapter(const T *data)
    : m_data(data) {
}

template <typename T>
void
StringAdapter<T>::doAdapterStuff() {
  /* Manipulate a string */
}

// Explicitly instantiate only the classes you want to be defined.
// In this case I only want the template to work with characters but
// I want to support both char and wchar_t with the same code.
template class StringAdapter<char>;
template class StringAdapter<wchar_t>;
