#include "gtest/gtest.h"
#include <buffer/Sink.h>

TEST(SinkTest, test) {
  sp::StaticByteBuffer<1024> b;
  sp::Sink s(b);
  flush(s);
}
