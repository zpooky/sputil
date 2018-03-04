#include "gtest/gtest.h"
#include <buffer/BytesView.h>
#include <buffer/CircularByteBuffer.h>
#include <io/file.h>

TEST(fileTest, test) {
  auto fd = fs::open_append("/tmp/test");
  // {
  //   sp::StaticBytesView<1024> b;
  //   fs::write(fd, b);
  // }
  {
    sp::BytesView b(nullptr, 0);
    fs::write(fd, b);
  }
  // {
  //   sp::StaticCircularByteBuffer<16> b;
  //   fs::write(fd, b);
  // }
  {
    sp::CircularByteBuffer b(nullptr, 0);
    fs::write(fd, b);
  }
  {
    unsigned char b[10];
    fs::write(fd, b);
  }
}
