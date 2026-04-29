/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the free functions for Graph Descriptors
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "PartGraph.h"
#include "descriptors_chain_descriptors.h"
#include <functional>

namespace steamrot::logic::descriptors {
/////////////////////////////////////////////////
/// @brief Predicate for the whole graph with no anchor node.
///
/// Any callable with signature @c bool(const PartGraph&) qualifies.
/// Derive instances via @c steamrot::descriptors::any_node_satisfies() or
/// @c steamrot::descriptors::all_nodes_satisfy() in
/// @c src/logic/descriptor_ops.h.
///
/// @c GraphDescriptor is the terminal type in the hierarchy — it is only
/// consumed by Logic/Action classes and is never passed back into a modifier.
/////////////////////////////////////////////////
using GraphDescriptor = std::function<bool(const PartGraph &)>;

/////////////////////////////////////////////////
/// @brief Build a GraphDescriptor that returns true when at least one node
///        in the graph satisfies @p cd.
///
/// @param cd ChainDescriptor (or ContextualNodeDescriptor) to evaluate.
/// @return GraphDescriptor returning true if any node satisfies @p cd.
/////////////////////////////////////////////////
inline GraphDescriptor any_node_satisfies(ChainDescriptor cd) {
  return [cd = std::move(cd)](const PartGraph &graph) -> bool {
    for (const auto &node : graph.nodes)
      if (cd(graph, node))
        return true;
    return false;
  };
}

/////////////////////////////////////////////////
/// @brief Build a GraphDescriptor that returns true when every node
///        in the graph satisfies @p cd.
///
/// @param cd ChainDescriptor (or ContextualNodeDescriptor) to evaluate.
/// @return GraphDescriptor returning true if all nodes satisfy @p cd.
/////////////////////////////////////////////////
inline GraphDescriptor all_nodes_satisfy(ChainDescriptor cd) {
  return [cd = std::move(cd)](const PartGraph &graph) -> bool {
    for (const auto &node : graph.nodes)
      if (!cd(graph, node))
        return false;
    return true;
  };
}
} // namespace steamrot::logic::descriptors
