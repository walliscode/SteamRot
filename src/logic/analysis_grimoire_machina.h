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
#include <cstdint>
#include <vector>

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
/// Graph query functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Look up a node by its part ID.
///
/// O(1) via @c PartGraph::node_index_by_id.
///
/// @param graph PartGraph to search.
/// @param id    Part ID to look up.
/// @return Pointer to the matching PartNode, or nullptr if not found.
/////////////////////////////////////////////////
const PartNode *find_node(const PartGraph &graph, uint32_t id);

/////////////////////////////////////////////////
/// @brief Return pointers to every edge incident to @p node.
///
/// @param graph PartGraph that owns @p node.
/// @param node  Node whose incident edges are returned.
/// @return Vector of non-owning PartEdge pointers.
/////////////////////////////////////////////////
std::vector<const PartEdge *> get_adjacent_edges(const PartGraph &graph,
                                                 const PartNode &node);

/////////////////////////////////////////////////
/// @brief Return the IDs of all nodes directly connected to @p node.
///
/// @param graph PartGraph that owns @p node.
/// @param node  Node whose neighbour IDs are returned.
/// @return Vector of neighbour part IDs.
/////////////////////////////////////////////////
std::vector<uint32_t> get_neighbor_ids(const PartGraph &graph,
                                       const PartNode &node);

/////////////////////////////////////////////////
/// @brief Return pointers to all nodes directly connected to @p node.
///
/// @param graph PartGraph that owns @p node.
/// @param node  Node whose neighbours are returned.
/// @return Vector of non-owning PartNode pointers.
/////////////////////////////////////////////////
std::vector<const PartNode *> get_neighbors(const PartGraph &graph,
                                            const PartNode &node);

/////////////////////////////////////////////////
/// @brief Return pointers to all nodes satisfying @p predicate.
///
/// @param graph     PartGraph to search.
/// @param predicate NodeDescriptor applied to each node.
/// @return Vector of non-owning pointers to matching nodes.
/////////////////////////////////////////////////
std::vector<const PartNode *>
find_nodes_matching(const PartGraph &graph, const NodeDescriptor &predicate);

/////////////////////////////////////////////////
/// @brief Count the nodes in @p graph satisfying @p predicate.
///
/// @param graph     PartGraph to search.
/// @param predicate NodeDescriptor applied to each node.
/// @return Number of matching nodes.
/////////////////////////////////////////////////
size_t count_nodes_matching(const PartGraph &graph,
                            const NodeDescriptor &predicate);

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
