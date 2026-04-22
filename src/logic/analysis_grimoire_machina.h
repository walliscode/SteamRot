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
/// @brief NodeDescriptor that returns true when the node has at least one
/// available socket.
/////////////////////////////////////////////////
extern const NodeDescriptor has_available_socket;

} // namespace steamrot::logic::analysis::grimoire_machina
