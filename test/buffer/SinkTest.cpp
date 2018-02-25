#include "gtest/gtest.h"
#include <buffer/Sink.h>

TEST(SinkTest, test) {
  sp::StaticCircularByteBuffer<1024> b;
  sp::Sink s(b, nullptr, nullptr);
  flush(s);
}
