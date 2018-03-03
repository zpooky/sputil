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

static std::size_t
remaining_read(std::size_t write, std::size_t read) noexcept {
  return write - read;
}

std::size_t
remaining_read(const CircularByteBuffer &b) noexcept {
  return remaining_read(b.write, b.read);
}

std::size_t
remaining_write(const CircularByteBuffer &b) noexcept {
  return b.capacity - length(b);
}

std::size_t
length(const CircularByteBuffer &b) noexcept {
  return remaining_read(b);
}

std::size_t
capacity(const CircularByteBuffer &b) noexcept {
  return b.capacity;
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
push_back(CircularByteBuffer &self, unsigned char c) noexcept {
  return push_back(self, &c, 1);
}

std::size_t
pop_front(CircularByteBuffer &self, BytesView &read) noexcept {
  auto res = pop_front(self, read.raw + read.pos, remaining_write(read));
  read.pos += res;
  return res;
}

std::size_t
pop_front(CircularByteBuffer &self, unsigned char *read,
          std::size_t l) noexcept {

  // std::size_t result = 0;
  // while (remaining_read(self) > 0) {
  //   if (result == l) {
  //     break;
  //   }
  //
  //   std::size_t r = index(self.read++, self.capacity);
  //   read[result++] = self.buffer[r];
  // }

  std::size_t result = peek_front(self,read,l);
  consume_bytes(self, result);

  return result;
}

std::size_t
pop_front(CircularByteBuffer &self, unsigned char &c) noexcept {
  return pop_front(self, &c, 1);
}

std::size_t
peek_front(const CircularByteBuffer &self, unsigned char *read,
           std::size_t l) noexcept {
  // std::size_t result = 0;
  // std::size_t r_idx = self.read;
  // while (remaining_read(r_idx, self.write) > 0) {
  //   if (result == l) {
  //     break;
  //   }
  //
  //   std::size_t r = index(r_idx++, self.capacity);
  //   read[result++] = self.buffer[r];
  // }

  // TODO
  sp::StaticArray<std::tuple<const unsigned char *, std::size_t>, 4> out;
  assert(read_buffer(self, out));
  // printf("length(out):%zu\n", out.length);

  std::size_t result = 0;
  for (std::size_t i = 0; i < out.length && l > 0; ++i) {
    std::size_t a_len = std::min(std::get<1>(out[i]), l);
    const unsigned char *const arr = std::get<0>(out[i]);

    std::memcpy(read+result, arr, a_len);
    result += a_len;
    l -= a_len;
  }

  return result;
}

std::size_t
peek_front(const CircularByteBuffer &self, BytesView &b) noexcept {
  std::size_t read = peek_front(self, b.raw + b.pos, remaining_read(b));
  b.pos += read;
  return read;
}

std::size_t
peek_front(const CircularByteBuffer &self, unsigned char &c) noexcept {
  return peek_front(self, &c, 1);
}

void
consume_bytes(CircularByteBuffer &self, std::size_t b) noexcept {
  assert(self.read + b <= self.write);
  self.read += b;
}

template <typename Buffer, typename Arr>
static bool
int_read_buffer(Buffer &self, Arr &result) {
  /*
   * read     write    read
   * |xxxxxxxx|........|xxxxxxxxxx|
   */
  std::size_t w = self.write;
  std::size_t r = self.read;
Lit:
  if (r < w) {
    const std::size_t bytes = remaining_read(w, r);
    const std::size_t r_idx = index(r, self.capacity);

    {
      const std::size_t l = std::min(bytes, capacity(self) - r_idx);

      // printf("bytes[%zu]|", bytes);
      // printf("min(length(self):%zu"
      //        ",capacity[%zu]-r_idx[%zu]):%zu"
      //        "= l[%zu]\n", //
      //        length(self), //
      //        capacity(self), r_idx,
      //        capacity(self) - r_idx, //
      //        l);
      if (l > 0) {
        // printf("insert(result, %c, length[%zu])\n", self.buffer[r_idx], l);
        auto out = insert(result, std::make_tuple(self.buffer + r_idx, l));
        assert(out != nullptr);
        r += l;
        goto Lit;
      }
    }
  }
  // printf("\n");

  return true;
}

bool
read_buffer(CircularByteBuffer &self,
            Array<std::tuple<unsigned char *, std::size_t>> &result) noexcept {
  return int_read_buffer(self, result);
}

bool
read_buffer(
    const CircularByteBuffer &self,
    Array<std::tuple<const unsigned char *, std::size_t>> &result) noexcept {
  return int_read_buffer(self, result);
}

} // namespace sp
