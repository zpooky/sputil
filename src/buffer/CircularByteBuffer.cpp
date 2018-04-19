#include "CircularByteBuffer.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <util/assert.h>

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

static std::size_t
remaining_write(std::size_t capacity, std::size_t length) noexcept {
  return capacity - length;
}

std::size_t
remaining_write(const CircularByteBuffer &b) noexcept {
  return remaining_write(b.capacity, length(b));
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

//-------------------+---------------
static std::size_t
push_back(CircularByteBuffer &self, const unsigned char *write,
          std::size_t l) noexcept {
  sp::StaticArray<std::tuple<unsigned char *, std::size_t>, 4> out;
  assertx(write_buffer(self, out));
  // printf("length(out):%zu\n", out.length);

  std::size_t result = 0;
  for (std::size_t i = 0; i < out.length && l > 0; ++i) {
    std::size_t a_len = std::min(std::get<1>(out[i]), l);
    unsigned char *const arr = std::get<0>(out[i]);

    std::memcpy(arr, write + result, a_len);
    result += a_len;
    l -= a_len;
  }
  produce_bytes(self, result);

  // std::size_t result = 0;
  // while (remaining_write(self) > 0) {
  //   if (result == l) {
  //     break;
  //   }
  //   std::size_t w = index(self.write++, self.capacity);
  //   self.buffer[w] = write[result++];
  // }

  return result;
} // namespace sp

std::size_t
push_back(CircularByteBuffer &self, BytesView &write) noexcept {
  auto res = push_back(self, write.raw + write.pos, remaining_read(write));
  write.pos += res;
  return res;
}

std::size_t
push_back(CircularByteBuffer &self, const void *write, std::size_t l) noexcept {
  return push_back(self, (const unsigned char *)write, l);
}

std::size_t
push_back(CircularByteBuffer &self, unsigned char c) noexcept {
  return push_back(self, &c, 1);
}

std::size_t
push_back(CircularByteBuffer &self, char c) noexcept {
  return push_back(self, &c, 1);
}
//-------------------+---------------

bool
write(CircularByteBuffer &self, const void *w, std::size_t l) noexcept {
  if (remaining_write(self) >= l) {
    assertx(push_back(self, w, l) == l);
    return true;
  }
  return false;
}

bool
write(CircularByteBuffer &self, unsigned char w) noexcept {
  return write(self, &w, 1);
}

bool
write(CircularByteBuffer &self, char c) noexcept {
  return write(self, &c, 1);
}
//-------------------+---------------

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

  std::size_t result = peek_front(self, read, l);
  consume_bytes(self, result);

  return result;
}

std::size_t
pop_front(CircularByteBuffer &self, unsigned char &c) noexcept {
  return pop_front(self, &c, 1);
}
//-------------------+---------------

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

  sp::StaticArray<std::tuple<const unsigned char *, std::size_t>, 4> out;
  assertx(read_buffer(self, out));

  std::size_t result = 0;
  for (std::size_t i = 0; i < out.length && l > 0; ++i) {
    std::size_t a_len = std::min(std::get<1>(out[i]), l);
    const unsigned char *const arr = std::get<0>(out[i]);

    std::memcpy(read + result, arr, a_len);
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
  assertx(self.read + b <= self.write);
  self.read += b;
}

template <typename Buffer, typename Arr>
static bool
int_read_buffer(Buffer &self, Arr &result) noexcept {
  assertx(remaining_write(result) >= 2);
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

      if (l > 0) {
        auto out = insert(result, std::make_tuple(self.buffer + r_idx, l));
        assertx(out != nullptr);
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

void
produce_bytes(CircularByteBuffer &self, std::size_t b) noexcept {
  assertx(remaining_write(self) >= b);

  self.write += b;
}

template <typename Buffer, typename Arr>
static bool
int_write_buffer(Buffer &self, Arr &result) noexcept {
  assertx(remaining_write(result) >= 2);
  /*
   * write     read    write
   * |xxxxxxxx|........|xxxxxxxxxx|
   */
  std::size_t w = self.write;
  const std::size_t r = self.read;
Lit:
  std::size_t writable = remaining_write(self.capacity, remaining_read(w, r));
  if (writable > 0) {

    const std::size_t w_idx = index(w, self.capacity);
    const std::size_t length_until_array_end = capacity(self) - w_idx;
    {

      const std::size_t l = std::min(writable, length_until_array_end);

      assertx(l > 0);
      auto out = insert(result, std::make_tuple(self.buffer + w_idx, l));
      assertx(out != nullptr);
      w += l;
      goto Lit;
    }
  }
  // printf("\n");

  return true;
}

bool
write_buffer(CircularByteBuffer &self,
             Array<std::tuple<unsigned char *, std::size_t>> &result) noexcept {
  return int_write_buffer(self, result);
}

bool
write_buffer(
    const CircularByteBuffer &self,
    Array<std::tuple<const unsigned char *, std::size_t>> &result) noexcept {
  return int_write_buffer(self, result);
}

} // namespace sp
