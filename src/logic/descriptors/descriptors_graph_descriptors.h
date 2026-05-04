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
#include <functional>

namespace steamrot::logic::descriptors {
/////////////////////////////////////////////////
/// @brief Predicate for the whole PartGraph with no anchor node.
///
/// Any callable with signature
/// @c GraphDescriptorResult(const PartGraph&) qualifies.
/// Derive instances via @c any_node_satisfies() or @c all_nodes_satisfy()
/// below. @c GraphDescriptor is the terminal type in the hierarchy — it is
/// only consumed by Logic/Action classes and is never passed back into a
/// modifier or combinator.
/////////////////////////////////////////////////
using GraphDescriptor = std::function<GraphDescriptorResult(const PartGraph &)>;

} // namespace steamrot::logic::descriptors
