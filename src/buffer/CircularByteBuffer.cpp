#include "CircularByteBuffer.h"
#include <algorithm>
#include <cstdio>
#include <cstring>

namespace sp {

CircularByteBuffer::CircularByteBuffer(unsigned char *b, std::size_t c) noexcept
    : buffer(b)
    , read(0)
    , write(0)
    , capacity(c) {
}

static std::size_t
index(std::size_t in, std::size_t capacity) noexcept {
  return in & (capacity - 1);
}

std::size_t
length(const CircularByteBuffer &b) noexcept {
  return b.write - b.read;
}

bool
is_empty(const CircularByteBuffer &b) noexcept {
  return b.read == b.write;
}

bool
is_full(const CircularByteBuffer &b) noexcept {
  return length(b) == b.capacity;
}

void
reset(CircularByteBuffer &b) noexcept {
  b.read = 0;
  b.write = 0;
}

std::size_t
remaining_write(const CircularByteBuffer &b) noexcept {
  return b.capacity - length(b);
}

std::size_t
remaining_read(const CircularByteBuffer &b) noexcept {
  return length(b);
}

std::size_t
push_back(CircularByteBuffer &self, BytesView &write) noexcept {
  auto res = push_back(self, write.raw + write.pos, remaining_read(write));
  write.pos += res;
  return res;
}

std::size_t
push_back(CircularByteBuffer &self, const unsigned char *write,
          std::size_t l) noexcept {
  // TODO make better version
  /*TODO
   *          read     write
   * |xxxxxxxx|........|xxxxxxxxxx|
   */

  // std::size_t result = std::min(l, wlen);
  // if (result > 0) {
  //   std::size_t w = index(self.write, self.capacity);
  //   printf("write[w[%zu],len[%zu]]\n", w, result);
  //   std::memcpy(self.buffer + w, write, result);
  //   self.write += result;
  // }

  // ==
  // std::size_t wlen = remaining_write(self);
  // while (wlen > 0) {
  //
  // }

  // std::size_t written = 0;
  // while (l > 0) {
  //   std::size_t w = index(self.write, self.capacity);
  //   std::size_t wlength(self.capacity - w);
  //   std::size_t r = index(self.read, self.capacity);
  //
  //   if (w < r) {
  //     wlength = r - w;
  //   }
  //   wlength = std::min(l, wlength);
  //   if (wlength == 0) {
  //     break;
  //   }
  //
  //   std::memcpy(#<{(|dest|)}># self.buffer + w, #<{(|src|)}># write,
  //   wlength); self.write += wlength; written += wlength; l -= wlength;
  // }

  // {
  //   std::size_t w = index(self.write, self.capacity);
  //   std::size_t r = index(self.read, self.capacity);
  //   if (w < r) {
  //     std::size_t wlen = r - w;
  //     memcpy(#<{(|DEST|)}># self.buffer + w, #<{(|SRC|)}># write, wlength);
  //     self.write += wlength;
  //     written += wlength;
  //   }
  // }

  // return written;
  // TODO memcpy
  std::size_t result = 0;
  while (remaining_write(self) > 0) {
    if (result == l) {
      break;
    }
    std::size_t w = index(self.write++, self.capacity);
    self.buffer[w] = write[result++];
  }

  return result;

  return 0;
} // namespace sp

std::size_t
pop_front(CircularByteBuffer &self, BytesView &read) noexcept {
  auto res = pop_front(self, read.raw + read.pos, remaining_write(read));
  read.pos += res;
  return res;
}

std::size_t
pop_front(CircularByteBuffer &self, unsigned char *read,
          std::size_t l) noexcept {
  // TODO make better version
  // std::size_t rlen = remaining_read(self);
  // std::size_t length = std::min(l, rlen);
  //
  // if (length > 0) {
  //   std::size_t r = index(self.read, self.capacity);
  //   printf("read[r[%zu],len[%zu]]\n", r, length);
  //   std::memcpy(read, self.buffer + r, length);
  //   self.read += length;
  // }

  // return length;

  std::size_t result = 0;
  while (remaining_read(self) > 0) {
    if (result == l) {
      break;
    }

    std::size_t r = index(self.read++, self.capacity);
    read[result++] = self.buffer[r];
  }

  return result;
}

bool
read_buffer(CircularByteBuffer &self,
            Array<std::tuple<unsigned char *, std::size_t>> &result) noexcept {
  /*
   * read     write    read
   * |xxxxxxxx|........|xxxxxxxxxx|
   */
  std::size_t w = self.write;
  std::size_t r = self.read;
Lit:
  if (r < w) {
    std::size_t len = w - r;
    auto out = insert(result, std::make_tuple(self.buffer + r, len));
    assert(out != nullptr);
  } else if (r != w) {
    std::size_t len = self.capacity - index(r, self.capacity);
    auto out = insert(result, std::make_tuple(self.buffer + r, len));
    assert(out != nullptr);


    {
      //TODO we are here
      r = r + len;
      goto Lit;
    }
  }
  return true;
}

} // namespace sp
