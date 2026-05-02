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
#include "DescriptorResult.h"
#include "MachinaFormScaffold.h"
#include "descriptors_chain_descriptors.h"
#include <functional>

namespace steamrot::logic::descriptors {
/////////////////////////////////////////////////
/// @brief Predicate for the whole scaffold with no anchor node.
///
/// Any callable with signature
/// @c GraphDescriptorResult(const MachinaFormScaffold&) qualifies.
/// Derive instances via @c any_node_satisfies() or @c all_nodes_satisfy()
/// below. @c GraphDescriptor is the terminal type in the hierarchy — it is
/// only consumed by Logic/Action classes and is never passed back into a
/// modifier or combinator.
/////////////////////////////////////////////////
using GraphDescriptor =
    std::function<GraphDescriptorResult(const MachinaFormScaffold &)>;

/////////////////////////////////////////////////
/// @brief Build a GraphDescriptor that returns true when at least one part
///        in the scaffold satisfies @p cd.
///
/// @param cd ChainDescriptor (or ContextualNodeDescriptor) to evaluate.
/// @return GraphDescriptor returning true if any part satisfies @p cd.
/////////////////////////////////////////////////
inline GraphDescriptor any_node_satisfies(ChainDescriptor cd) {
  return [cd = std::move(cd)](
             const MachinaFormScaffold &scaffold) -> GraphDescriptorResult {
    for (const auto &[id, variant] : scaffold.parts)
      if (cd(scaffold, id))
        return GraphDescriptorResult{true};
    return GraphDescriptorResult{false};
  };
}

/////////////////////////////////////////////////
/// @brief Build a GraphDescriptor that returns true when every part
///        in the scaffold satisfies @p cd.
///
/// @param cd ChainDescriptor (or ContextualNodeDescriptor) to evaluate.
/// @return GraphDescriptor returning true if all parts satisfy @p cd.
/////////////////////////////////////////////////
inline GraphDescriptor all_nodes_satisfy(ChainDescriptor cd) {
  return [cd = std::move(cd)](
             const MachinaFormScaffold &scaffold) -> GraphDescriptorResult {
    for (const auto &[id, variant] : scaffold.parts)
      if (!cd(scaffold, id))
        return GraphDescriptorResult{false};
    return GraphDescriptorResult{true};
  };
}
} // namespace steamrot::logic::descriptors
