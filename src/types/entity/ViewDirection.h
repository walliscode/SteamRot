/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ViewDirection enum class. No implementation is
/// needed
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics/VertexArray.hpp>
#include <array>
#include <cstdint>

namespace steamrot {

enum class ViewDirection : std::uint8_t {
  Front = 0,
  Back = 1,
  // ... more directions, keep them contiguous ...
  Count
};

constexpr std::size_t to_index(ViewDirection d) {
  return static_cast<std::size_t>(d);
}

// Customization point: clear an entry while preserving its configuration
// (e.g., sf::VertexArray primitive type).
template <class T> constexpr void clear_viewtable_entry(T &v) {
  // Fallback: value-reset for unknown types
  v = T{};
}

constexpr void clear_viewtable_entry(sf::VertexArray &va) {
  // sf::VertexArray::clear() preserves primitive type
  va.clear();
}

template <std::size_t N>
constexpr void clear_viewtable_entry(std::array<sf::VertexArray, N> &arr) {
  for (auto &va : arr) {
    va.clear(); // preserves primitive type
  }
}

template <class T> struct ViewTable {
  static constexpr std::size_t N = to_index(ViewDirection::Count);
  std::array<T, N> data{};

  constexpr T &operator[](ViewDirection d) { return data[to_index(d)]; }
  constexpr const T &operator[](ViewDirection d) const {
    return data[to_index(d)];
  }

  // Clears stored geometry while preserving per-entry configuration.
  constexpr void clear() {
    for (auto &entry : data) {
      clear_viewtable_entry(entry);
    }
  }
};

using Views = ViewTable<sf::VertexArray>;

constexpr std::size_t kCompositeSlots = 8;

using CompositeView = std::array<sf::VertexArray, kCompositeSlots>;
using CompositeViews = ViewTable<CompositeView>;

} // namespace steamrot
