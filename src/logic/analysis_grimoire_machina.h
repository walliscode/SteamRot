/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for analysing a MachinaFormScaffold
/// as a PartGraph.
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

} // namespace steamrot::logic::analysis::grimoire_machina

namespace steamrot::logic::analysis::grimoire_machina::node_descriptor {

/////////////////////////////////////////////////
/// @brief Return true if the node's instance is a FragmentInstance.
///
/// @param node PartNode to query.
/// @return True when the variant holds a FragmentInstance.
/////////////////////////////////////////////////
bool is_fragment(const PartNode &node);

/////////////////////////////////////////////////
/// @brief Return true if the node's instance is a JointInstance.
///
/// @param node PartNode to query.
/// @return True when the variant holds a JointInstance.
/////////////////////////////////////////////////
bool is_joint(const PartNode &node);

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
/// @brief Return true if the node's instance has at least one available
/// socket.
///
/// @param node PartNode to query.
/// @return True when at least one SocketData has state SocketState::Available.
/////////////////////////////////////////////////
bool has_available_socket(const PartNode &node);

} // namespace steamrot::logic::analysis::grimoire_machina::node_descriptor
