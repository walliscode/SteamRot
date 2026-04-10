/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ViewDirection enum class and ViewTable template.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics/VertexArray.hpp>
#include <algorithm>
#include <array>
#include <cstdint>

namespace steamrot {

enum class ViewDirection : std::uint8_t {
  Front = 0,
  Back = 1,
  // ... more directions, keep them contiguous ...
  Count
};

/////////////////////////////////////////////////
/// @brief Convert a ViewDirection to an array index.
///
/// @param d ViewDirection to convert.
/// @return Corresponding index into a ViewTable's data array.
/////////////////////////////////////////////////
constexpr std::size_t to_index(ViewDirection d) {
  return static_cast<std::size_t>(d);
}

/////////////////////////////////////////////////
/// @brief Customization point: clear an entry while preserving its
/// configuration (e.g., sf::VertexArray primitive type).
///
/// The primary template performs a value-reset. Specialize for types that
/// have a cheaper "clear without re-construction" operation.
///
/// @param v Entry to clear.
/////////////////////////////////////////////////
template <class T> constexpr void clear_viewtable_entry(T &v) {
  // Fallback: value-reset for unknown types
  v = T{};
}

/////////////////////////////////////////////////
/// @brief Specialization for sf::VertexArray: calls clear() to preserve the
/// primitive type.
///
/// @param va VertexArray to clear.
/////////////////////////////////////////////////
constexpr void clear_viewtable_entry(sf::VertexArray &va) {
  // sf::VertexArray::clear() preserves primitive type
  va.clear();
}

/////////////////////////////////////////////////
/// @brief Specialization for std::array<sf::VertexArray, N>: clears each
/// element while preserving primitive types.
///
/// @param arr Array of VertexArrays to clear.
/////////////////////////////////////////////////
template <std::size_t N>
constexpr void clear_viewtable_entry(std::array<sf::VertexArray, N> &arr) {
  for (auto &va : arr) {
    va.clear(); // preserves primitive type
  }
}

/////////////////////////////////////////////////
/// @brief A fixed-size lookup table indexed by ViewDirection.
///
/// Provides both direct array access and a std::map-compatible interface
/// (size, count, contains, insert_or_assign) so that only explicitly inserted
/// entries are considered "populated".
///
/// @tparam T Element type stored per direction.
/////////////////////////////////////////////////
template <class T> struct ViewTable {
  static constexpr std::size_t N = to_index(ViewDirection::Count);

  /////////////////////////////////////////////////
  /// @brief Underlying storage for each ViewDirection slot.
  /////////////////////////////////////////////////
  std::array<T, N> data{};

private:
  /////////////////////////////////////////////////
  /// @brief Tracks which slots have been explicitly populated.
  /////////////////////////////////////////////////
  std::array<bool, N> m_populated{};

public:
  /////////////////////////////////////////////////
  /// @brief Direct read/write access by ViewDirection (does not mark as
  /// populated).
  ///
  /// @param d ViewDirection index.
  /// @return Reference to the stored element.
  /////////////////////////////////////////////////
  constexpr T &operator[](ViewDirection d) { return data[to_index(d)]; }

  /////////////////////////////////////////////////
  /// @brief Direct read-only access by ViewDirection.
  ///
  /// @param d ViewDirection index.
  /// @return Const reference to the stored element.
  /////////////////////////////////////////////////
  constexpr const T &operator[](ViewDirection d) const {
    return data[to_index(d)];
  }

  /////////////////////////////////////////////////
  /// @brief Insert or replace the entry for a direction, marking it populated.
  ///
  /// @param d     ViewDirection key.
  /// @param value Value to store (moved into the slot).
  /////////////////////////////////////////////////
  void insert_or_assign(ViewDirection d, T &&value) {
    data[to_index(d)] = std::move(value);
    m_populated[to_index(d)] = true;
  }

  /////////////////////////////////////////////////
  /// @brief Insert or replace the entry for a direction, marking it populated.
  ///
  /// @param d     ViewDirection key.
  /// @param value Value to store (copied into the slot).
  /////////////////////////////////////////////////
  void insert_or_assign(ViewDirection d, const T &value) {
    data[to_index(d)] = value;
    m_populated[to_index(d)] = true;
  }

  /////////////////////////////////////////////////
  /// @brief Returns the number of explicitly populated entries.
  ///
  /// @return Count of populated entries.
  /////////////////////////////////////////////////
  std::size_t size() const {
    return static_cast<std::size_t>(
        std::count(m_populated.begin(), m_populated.end(), true));
  }

  /////////////////////////////////////////////////
  /// @brief Returns the total number of direction slots (compile-time
  /// capacity).
  ///
  /// @return N (ViewDirection::Count as an index).
  /////////////////////////////////////////////////
  constexpr std::size_t capacity() const { return N; }

  /////////////////////////////////////////////////
  /// @brief Returns whether any entry has been populated.
  ///
  /// @return true if no entries are populated.
  /////////////////////////////////////////////////
  bool empty() const { return size() == 0; }

  /////////////////////////////////////////////////
  /// @brief Returns 1 if the entry for direction d is populated, 0 otherwise.
  ///
  /// Mirrors std::map::count semantics.
  ///
  /// @param d ViewDirection to query.
  /// @return 1 if populated, 0 otherwise.
  /////////////////////////////////////////////////
  std::size_t count(ViewDirection d) const {
    return m_populated[to_index(d)] ? 1 : 0;
  }

  /////////////////////////////////////////////////
  /// @brief Returns whether the entry for direction d has been populated.
  ///
  /// @param d ViewDirection to query.
  /// @return true if populated.
  /////////////////////////////////////////////////
  bool contains(ViewDirection d) const { return m_populated[to_index(d)]; }

  /////////////////////////////////////////////////
  /// @brief Iterator to the beginning of the underlying data array.
  ///
  /// Iterates over all N slots regardless of populated state.
  /////////////////////////////////////////////////
  auto begin() { return data.begin(); }

  /////////////////////////////////////////////////
  /// @brief Iterator to the end of the underlying data array.
  /////////////////////////////////////////////////
  auto end() { return data.end(); }

  /////////////////////////////////////////////////
  /// @brief Const iterator to the beginning of the underlying data array.
  /////////////////////////////////////////////////
  auto begin() const { return data.begin(); }

  /////////////////////////////////////////////////
  /// @brief Const iterator to the end of the underlying data array.
  /////////////////////////////////////////////////
  auto end() const { return data.end(); }

  /////////////////////////////////////////////////
  /// @brief Clears all stored geometry and resets the populated flags.
  ///
  /// Calls clear_viewtable_entry on each slot (preserving primitive type for
  /// sf::VertexArray entries) and marks all slots as unpopulated.
  /////////////////////////////////////////////////
  void clear() {
    for (auto &entry : data) {
      clear_viewtable_entry(entry);
    }
    m_populated = {};
  }
};

using Views = ViewTable<sf::VertexArray>;

constexpr std::size_t kCompositeSlots = 8;

using CompositeView = std::array<sf::VertexArray, kCompositeSlots>;
using CompositeViews = ViewTable<CompositeView>;

} // namespace steamrot
