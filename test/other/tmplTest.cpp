#include "tmpl.h"
#include "gtest/gtest.h"
// #include <>

TEST(tmplTest, test) {
  {
    int i = 1;
    TMPL<int> tmpl;
    tmpl.ptr = &i;

    // int *p = test_tmpl(tmpl);
    // ASSERT_EQ(*p, 1);
  }
  {
    double i = 1;
    TMPL<double> tmpl;
    tmpl.ptr = &i;

    auto *p = test_tmpl(tmpl);
    ASSERT_EQ(*p, 1);
  }
  // {
  //   float i = 1;
  //   TMPL<float> tmpl;
  //   tmpl.ptr = &i;
  //   //
  //   //   // int *p = test_tmpl(tmpl);
  //   //   // ASSERT_EQ(*p, 1);
  // }

  StringAdapter<char> s("nullptr");
  StringAdapter<wchar_t> s1(L"nullptr");
}
