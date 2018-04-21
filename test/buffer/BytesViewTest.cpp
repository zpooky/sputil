#include "gtest/gtest.h"
#include <buffer/BytesView.h>

TEST(ByteBufferTest, test) {
  sp::StaticBytesView<1024> b;
  sp::ConstBytesView b2(nullptr, 0);
  sp::BytesView b3(nullptr, 0);
  sp::IBytesView<char> b4(nullptr, 0);
  sp::IBytesView<const char> b5(nullptr, 0);
}
