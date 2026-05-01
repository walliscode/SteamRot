/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the free functions for general-purpose
/// descriptors/helpers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "PartGraph.h"

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @brief Build a PartGraph from a MachinaFormScaffold.
///
/// Single O(N+E) pass: creates one PartNode per part (populating
/// @c node_index_by_id), then creates one PartEdge per connection (pushing
/// the edge index into both endpoint nodes' @c edge_indices). All nodes and
/// edges hold non-owning pointers into @p scaffold, which must outlive the
/// returned PartGraph.
///
/// @param scaffold Source scaffold to build the graph from.
/// @return PartGraph with one node per part, one edge per connection, and
///         fully populated @c node_index_by_id and @c edge_indices.
/////////////////////////////////////////////////
PartGraph build_part_graph(const MachinaFormScaffold &scaffold);

////////////////////////////////////////////////
/// Lifting utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Graph-level query builders
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Generic combinators
///
/// Work uniformly with NodeDescriptor, ContextualNodeDescriptor,
/// ChainDescriptor, and GraphDescriptor. Template deduction ensures that
/// both arguments must be the same descriptor level; mixing levels is a
/// compile error.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Return a descriptor that is true when both @p a and @p b are true.
///
/// @tparam Desc Any descriptor type.
/// @param a First descriptor.
/// @param b Second descriptor.
/// @return Combined descriptor returning a(...) && b(...).
/////////////////////////////////////////////////
template <typename Desc> Desc and_(Desc a, Desc b) {
  return [a = std::move(a), b = std::move(b)](auto &&...args) {
    auto ra = a(std::forward<decltype(args)>(args)...);
    return std::decay_t<decltype(ra)>{
        static_cast<bool>(ra) &&
        static_cast<bool>(b(std::forward<decltype(args)>(args)...))};
  };
}

/////////////////////////////////////////////////
/// @brief Return a descriptor that is true when either @p a or @p b is true.
///
/// @tparam Desc Any descriptor type.
/// @param a First descriptor.
/// @param b Second descriptor.
/// @return Combined descriptor returning a(...) || b(...).
/////////////////////////////////////////////////
template <typename Desc> Desc or_(Desc a, Desc b) {
  return [a = std::move(a), b = std::move(b)](auto &&...args) {
    auto ra = a(std::forward<decltype(args)>(args)...);
    return std::decay_t<decltype(ra)>{
        static_cast<bool>(ra) ||
        static_cast<bool>(b(std::forward<decltype(args)>(args)...))};
  };
}

/////////////////////////////////////////////////
/// @brief Return a descriptor that negates @p a.
///
/// @tparam Desc Any descriptor type.
/// @param a Descriptor to negate.
/// @return Descriptor returning !a(...).
/////////////////////////////////////////////////
template <typename Desc> Desc not_(Desc a) {
  return [a = std::move(a)](auto &&...args) {
    auto ra = a(std::forward<decltype(args)>(args)...);
    return std::decay_t<decltype(ra)>{!static_cast<bool>(ra)};
  };
}
} // namespace steamrot::logic::descriptors
