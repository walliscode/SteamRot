/////////////////////////////////////////////////
/// @file
/// @brief Declaration of analysis utilities for a MachinaFormScaffold
/// as a PartGraph, including free functions, NodeDescriptor and EdgeDescriptor
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
#include <functional>
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
/// @brief NodeDescriptor that returns true when the node has at least one
/// available socket.
/////////////////////////////////////////////////
extern const NodeDescriptor has_available_socket;

/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when the node has at
/// most @p n connected sockets.
///
/// @param n Maximum number of connected sockets.
/// @return NodeDescriptor returning true when connected-socket count <= n.
/////////////////////////////////////////////////
NodeDescriptor has_maximum_n_connected_sockets(size_t n);

/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true for nodes with at most 1 connected
/// socket (i.e., at the "end" of a chain of connections).
/////////////////////////////////////////////////
extern const NodeDescriptor is_terminal;

/////////////////////////////////////////////////
/// EdgeDescriptor
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Predicate type for single-edge queries on a PartGraph.
///
/// Any callable with signature @c bool(const PartEdge&) qualifies.
/// Use @c connects_fragments, @c connects_joints, or
/// @c connects_fragment_to_joint factory functions to obtain ready-made
/// descriptors for common edge-type queries.
/////////////////////////////////////////////////
using EdgeDescriptor = std::function<bool(const PartEdge &)>;

/////////////////////////////////////////////////
/// @brief Returns an EdgeDescriptor that is true when both endpoints are
/// FragmentInstances.
///
/// @param graph PartGraph used to look up endpoint node types. Must outlive
///              the returned descriptor.
/// @return EdgeDescriptor returning true for fragment–fragment edges.
/////////////////////////////////////////////////
EdgeDescriptor connects_fragments(const PartGraph &graph);

/////////////////////////////////////////////////
/// @brief Returns an EdgeDescriptor that is true when both endpoints are
/// JointInstances.
///
/// @param graph PartGraph used to look up endpoint node types. Must outlive
///              the returned descriptor.
/// @return EdgeDescriptor returning true for joint–joint edges.
/////////////////////////////////////////////////
EdgeDescriptor connects_joints(const PartGraph &graph);

/////////////////////////////////////////////////
/// @brief Returns an EdgeDescriptor that is true when one endpoint is a
/// FragmentInstance and the other is a JointInstance (either order).
///
/// @param graph PartGraph used to look up endpoint node types. Must outlive
///              the returned descriptor.
/// @return EdgeDescriptor returning true for fragment–joint edges.
/////////////////////////////////////////////////
EdgeDescriptor connects_fragment_to_joint(const PartGraph &graph);

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
/// @brief Return true when all nodes in @p graph are mutually reachable.
///
/// Uses BFS from the first node. An empty graph is trivially connected.
///
/// @param graph PartGraph to test.
/// @return true if the graph is connected, false otherwise.
/////////////////////////////////////////////////
bool is_connected(const PartGraph &graph);

/////////////////////////////////////////////////
/// @brief Visit every node reachable from @p start_id exactly once via BFS.
///
/// @param graph    PartGraph to traverse.
/// @param start_id ID of the node to start from; no-op if not found.
/// @param visitor  Callable invoked once per visited node.
/////////////////////////////////////////////////
void bfs(const PartGraph &graph, uint32_t start_id,
         std::function<void(const PartNode &)> visitor);

/////////////////////////////////////////////////
/// Predicate combinators
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Return a new descriptor that is true when both @p a and @p b are
/// true.
///
/// Works for any descriptor type (NodeDescriptor or EdgeDescriptor).
///
/// @param a First descriptor.
/// @param b Second descriptor.
/// @return Combined descriptor returning @c a(x) && b(x).
/////////////////////////////////////////////////
template <typename Descriptor>
Descriptor and_(Descriptor a, Descriptor b) {
  return [a = std::move(a), b = std::move(b)](const auto &x) {
    return a(x) && b(x);
  };
}

/////////////////////////////////////////////////
/// @brief Return a new descriptor that is true when either @p a or @p b is
/// true.
///
/// Works for any descriptor type (NodeDescriptor or EdgeDescriptor).
///
/// @param a First descriptor.
/// @param b Second descriptor.
/// @return Combined descriptor returning @c a(x) || b(x).
/////////////////////////////////////////////////
template <typename Descriptor>
Descriptor or_(Descriptor a, Descriptor b) {
  return [a = std::move(a), b = std::move(b)](const auto &x) {
    return a(x) || b(x);
  };
}

/////////////////////////////////////////////////
/// @brief Return a new descriptor that negates @p a.
///
/// Works for any descriptor type (NodeDescriptor or EdgeDescriptor).
///
/// @param a Descriptor to negate.
/// @return Descriptor returning @c !a(x).
/////////////////////////////////////////////////
template <typename Descriptor>
Descriptor not_(Descriptor a) {
  return [a = std::move(a)](const auto &x) { return !a(x); };
}

} // namespace steamrot::logic::analysis::grimoire_machina
