/////////////////////////////////////////////////
/// @file
/// @brief Declaration of analysis utilities for a MachinaFormScaffold
/// as a PartGraph, including free functions and NodeDescriptor predicates.
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
/// Iterates @p scaffold.parts to create one PartNode per entry, then
/// iterates @p scaffold.connections to create one PartEdge per connection.
/// All nodes and edges hold non-owning pointers into @p scaffold, which
/// must outlive the returned PartGraph.
///
/// @param scaffold Source scaffold to build the graph from.
/// @return PartGraph with one node per part and one edge per connection.
/////////////////////////////////////////////////
PartGraph build_part_graph(const MachinaFormScaffold &scaffold);

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
/// @brief Return true if the node has no edges in the graph.
///
/// @param node  PartNode to test.
/// @param graph PartGraph containing the edge list to search.
/// @return True when no edge in @p graph references @p node.id.
/////////////////////////////////////////////////
bool is_isolated(const PartNode &node, const PartGraph &graph);

/////////////////////////////////////////////////
/// @brief Count the edges that reference this node.
///
/// An edge is counted once for each endpoint that matches @p node.id,
/// so a self-loop would count twice.
///
/// @param node  PartNode to count edges for.
/// @param graph PartGraph containing the edge list to search.
/// @return Number of edge endpoints matching @p node.id.
/////////////////////////////////////////////////
size_t edge_count(const PartNode &node, const PartGraph &graph);

/////////////////////////////////////////////////
/// @brief Return the number of sockets on this node's instance.
///
/// @param node PartNode to query.
/// @return Size of the instance's sockets vector.
/////////////////////////////////////////////////
size_t socket_count(const PartNode &node);

/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true when the node has at least one
/// available socket.
/////////////////////////////////////////////////
extern const NodeDescriptor has_available_socket;

} // namespace steamrot::logic::analysis::grimoire_machina
