#include <cstddef>
#include <cstdint>

// TODO document
namespace sp {
//=====================================
struct RabinKarpHash {
  std::size_t length;
  const std::size_t capacity;
  std::uint32_t hash;
  char *buffer;

  explicit RabinKarpHash(std::size_t c) noexcept;
  ~RabinKarpHash() noexcept;
};

//=====================================
const char *
update(RabinKarpHash &self, const char *begin, const char *end) noexcept;

std::size_t
update(RabinKarpHash &self, const char *buffer, std::size_t len) noexcept;

//=====================================
std::uint32_t
hash(const RabinKarpHash &self) noexcept;

//=====================================
} // namespace sp
