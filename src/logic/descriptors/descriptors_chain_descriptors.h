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
#include "DescriptorResult.h"
#include "MachinaFormScaffold.h"
#include "descriptors_node_descriptors.h"
#include <cstdint>
#include <functional>

namespace steamrot::logic::descriptors {
/////////////////////////////////////////////////
/// @brief Predicate for a multi-hop walk starting from one anchor node.
///
/// Any callable with signature
/// @c ChainDescriptorResult(const PartGraph&, uint32_t part_id)
/// qualifies. The distinction from @c ContextualNodeDescriptor is semantic:
/// a @c ChainDescriptor answers a structural question about a subgraph
/// reachable from the start node (e.g. "does a 3-node linear chain begin
/// here?") rather than a question about the start node alone.
///
/// Build instances with @c steamrot::logic::ChainDescriptorBuilder
/// (see @c ChainDescriptorBuilder.h in @c src/logic/).
/////////////////////////////////////////////////
using ChainDescriptor =
    std::function<ChainDescriptorResult(const PartGraph &, uint32_t)>;

/////////////////////////////////////////////////
/// @brief Lift a NodeDescriptor to a ChainDescriptor.
///
/// Wraps @p nd so that its @c NodeDescriptorResult is promoted to
/// @c ChainDescriptorResult. No graph walk is performed; the anchor node is
/// the only node evaluated. Use when a function requires a ChainDescriptor
/// but your predicate only examines the anchor.
///
/// @param nd NodeDescriptor to lift.
/// @return ChainDescriptor that delegates to @p nd.
/////////////////////////////////////////////////
inline ChainDescriptor lift_to_chain(NodeDescriptor nd) {
  return [nd = std::move(nd)](const PartGraph &parts,
                               uint32_t id) -> ChainDescriptorResult {
    return ChainDescriptorResult{static_cast<bool>(nd(parts, id))};
  };
}
} // namespace steamrot::logic::descriptors
