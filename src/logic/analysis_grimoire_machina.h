/////////////////////////////////////////////////
/// @file
/// @brief Declaration of analysis utilities for a MachinaFormScaffold
/// as a PartGraph, including free functions, NodeDescriptor
/// predicates, graph traversal functions, and predicate combinators.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "MachinaFormScaffold.h"
#include "PartGraph.h"
#include <cstddef>

namespace steamrot::logic::analysis::grimoire_machina {

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

/////////////////////////////////////////////////
/// NodeDescriptor predicates
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true when the node holds a
/// FragmentInstance.
/////////////////////////////////////////////////
extern const NodeDescriptor is_fragment;

/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true when the node holds a
/// JointInstance.
/////////////////////////////////////////////////
extern const NodeDescriptor is_joint;

/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when the node has exactly
/// @p n
///
/// @param n Number of edges.
/// @return NodeDescriptor returning true when edges count == n.
/////////////////////////////////////////////////
NodeDescriptor has_exactly_n_edges(size_t n);

/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when the node has at
/// most @p n PartEdges (i.e., at most @p n connected sockets).
///
/// @param n Maximum number of edges.
/// @return NodeDescriptor returning true when edges count <= n.
/////////////////////////////////////////////////
NodeDescriptor has_maximum_n_edges(size_t n);

/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true for nodes with at most 1 connected
/// socket (i.e., at the "end" of a chain of connections).
/////////////////////////////////////////////////
extern const NodeDescriptor is_terminal;

/////////////////////////////////////////////////
/// Predicate combinators
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Return a new descriptor that is true when both @p a and @p b are
/// true.
///
/// @param a First descriptor.
/// @param b Second descriptor.
/// @return Combined descriptor returning @c a(x) && b(x).
/////////////////////////////////////////////////
template <typename NodeDescriptor>
NodeDescriptor and_(NodeDescriptor a, NodeDescriptor b) {
  return [a = std::move(a), b = std::move(b)](const PartNode &part_node) {
    return a(part_node) && b(part_node);
  };
}

/////////////////////////////////////////////////
/// @brief Return a new descriptor that is true when either @p a or @p b is
/// true.
///
/// @param a First descriptor.
/// @param b Second descriptor.
/// @return Combined descriptor returning @c a(x) || b(x).
/////////////////////////////////////////////////
template <typename Descriptor> Descriptor or_(Descriptor a, Descriptor b) {
  return [a = std::move(a), b = std::move(b)](const PartNode &part_node) {
    return a(part_node) || b(part_node);
  };
}

/////////////////////////////////////////////////
/// @brief Return a new descriptor that negates @p a.
///
/// @param a Descriptor to negate.
/// @return Descriptor returning @c !a(x).
/////////////////////////////////////////////////
template <typename NodeDescriptor> NodeDescriptor not_(NodeDescriptor a) {
  return
      [a = std::move(a)](const PartNode &part_node) { return !a(part_node); };
}

} // namespace steamrot::logic::analysis::grimoire_machina
