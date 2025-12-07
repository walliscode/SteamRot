/////////////////////////////////////////////////
/// @file
/// @brief Generic type trait utilities for tuple manipulation
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <tuple>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief template helper struct for component containers
/////////////////////////////////////////////////
template <typename... Components> struct ComponentContainer;

/////////////////////////////////////////////////
/// @brief template specialisation for ComponentContainer using Tuples
/////////////////////////////////////////////////
template <typename... Components>
struct ComponentContainer<std::tuple<Components...>> {
  using ComponentVectorTuple = std::tuple<std::vector<Components>...>;
};

/////////////////////////////////////////////////
/// @brief template helper for type indexing in tuples
/////////////////////////////////////////////////
template <typename T, typename... Ts> struct IndexOf;

/////////////////////////////////////////////////
/// @brief template specialisation for tuples - base case recursion
/////////////////////////////////////////////////
template <typename T, typename... Ts> 
struct IndexOf<T, std::tuple<T, Ts...>> {
  // base case where T is the first type in the tuple
  static constexpr size_t value = 0;
};

/////////////////////////////////////////////////
/// @brief template specialisation for tuples - recursive case
/////////////////////////////////////////////////
template <typename T, typename U, typename... Ts>
struct IndexOf<T, std::tuple<U, Ts...>> {
  // recursive case where T is not the first type in the tuple
  static constexpr size_t value = 1 + IndexOf<T, std::tuple<Ts...>>::value;
};

/////////////////////////////////////////////////
/// @brief helper type alias for getting the index of a type in a tuple
/////////////////////////////////////////////////
template <typename T, typename Tuple>
constexpr size_t TupleTypeIndex = IndexOf<T, Tuple>::value;

} // namespace steamrot
