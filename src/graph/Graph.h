#ifndef SP_UTIL_GRAPH_GRAPH_H
#define SP_UTIL_GRAPH_GRAPH_H

namespace graph {
//=====================================
template <typename Vertex>
struct Wrapper {
  using edge_type = Vertex;
  Vertex *ptr;
  bool owner;

  explicit Wrapper(const edge_type *) noexcept;
  Wrapper(Wrapper<Vertex> &&) noexcept;

  Wrapper(const Wrapper<Vertex> &) = delete;
  Wrapper<Vertex> &
  operator=(const Wrapper<Vertex> &) = delete;

  Wrapper<Vertex> &
  operator=(Wrapper<Vertex> &&o) noexcept;

  bool
  operator>(const Wrapper<Vertex> &) const noexcept;

  bool
  operator>(const Wrapper<Vertex> *) const noexcept;

  bool
  operator>(const edge_type *) const noexcept;

  bool
  operator>(const edge_type &) const noexcept;

  bool
  operator==(const edge_type *) const noexcept;

  ~Wrapper() noexcept;
};

} // namespace graph

#endif
