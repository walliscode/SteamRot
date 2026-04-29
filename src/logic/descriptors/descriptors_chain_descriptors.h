/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the free functions for Chain Descriptors
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "PartGraph.h"
#include "descriptors_node_descriptors.h"
#include <functional>

namespace steamrot::logic::descriptors {
/////////////////////////////////////////////////
/// @brief Predicate for a multi-hop walk starting from one anchor node.
///
/// Shares the underlying signature with @c ContextualNodeDescriptor.
/// The distinction is semantic: a @c ChainDescriptor answers a structural
/// question about a subgraph reachable from the start node (e.g. "does a
/// 3-node linear chain begin here?") rather than a question about the
/// start node alone.
///
/// Build instances with @c steamrot::logic::ChainDescriptorBuilder
/// (see @c ChainDescriptorBuilder.h in @c src/logic/).
/////////////////////////////////////////////////
using ChainDescriptor =
    std::function<bool(const PartGraph &, const PartNode &)>;

/////////////////////////////////////////////////
/// @brief Lift a NodeDescriptor to a ChainDescriptor.
///
/// Equivalent to lifting to ContextualNodeDescriptor — the start node is
/// evaluated against @p nd and no walk is performed. Use when a function
/// requires a ChainDescriptor but your predicate only examines the anchor.
///
/// @param nd NodeDescriptor to lift.
/// @return ChainDescriptor that delegates to @p nd.
/////////////////////////////////////////////////
inline ChainDescriptor lift_to_chain(NodeDescriptor nd) {
  return
      [nd = std::move(nd)](const PartGraph & /*graph*/,
                           const PartNode &node) -> bool { return nd(node); };
}
} // namespace steamrot::logic::descriptors
