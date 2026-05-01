/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the free functions for Node Descriptors
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "DescriptorResult.h"
#include "PartGraph.h"
#include <functional>
namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @brief Predicate type for single-node queries on a PartGraph.
///
/// Any callable with signature
/// @c NodeDescriptorResult(const PartNode&) qualifies.
/// @c is_fragment, @c is_joint, and @c has_available_socket in
/// @c steamrot::logic::analysis::grimoire_machina are declared as
/// @c const @c NodeDescriptor variables and can be used directly or
/// assigned to other @c NodeDescriptor instances.
///
/// Example:
/// @code
/// NodeDescriptor predicate = agm::is_fragment;
/// NodeDescriptorResult result = predicate(node);
/// @endcode
/////////////////////////////////////////////////
using NodeDescriptor = std::function<NodeDescriptorResult(const PartNode &)>;

/////////////////////////////////////////////////
/// @brief Predicate for a single node with access to the whole graph.
///
/// Any callable with signature
/// @c NodeDescriptorResult(const PartGraph&, const PartNode&) qualifies.
/// Use when the predicate needs to examine neighbouring nodes via the graph
/// but does not need to walk further than one hop from the anchor.
///
/// Obtain instances from the modifier free functions in
/// @c steamrot::descriptors (see @c descriptor_ops.h in @c src/logic/).
/////////////////////////////////////////////////
using ContextualNodeDescriptor =
    std::function<NodeDescriptorResult(const PartGraph &, const PartNode &)>;

/////////////////////////////////////////////////
/// @brief Lift a NodeDescriptor to a ContextualNodeDescriptor.
///
/// The PartGraph argument is ignored; the wrapped descriptor is applied to
/// the node alone. Use when a function requires a ContextualNodeDescriptor
/// but you only need to examine the node's own data.
///
/// @param nd NodeDescriptor to lift.
/// @return ContextualNodeDescriptor that delegates to @p nd.
/////////////////////////////////////////////////
inline ContextualNodeDescriptor lift(NodeDescriptor nd) {
  return [nd = std::move(nd)](const PartGraph & /*graph*/,
                              const PartNode &node) -> NodeDescriptorResult {
    return nd(node);
  };
}
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
/// @p n PartEdges (i.e., exactly @p n connected sockets).
///
/// @param n Number of edges.
/// @return NodeDescriptor returning true when edges count == n.
/////////////////////////////////////////////////
NodeDescriptor has_exactly_n_edges(size_t n);

/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when the node has exactly
/// 2 PartEdges
///
/// serial is used to indicate part of a serial chain
/////////////////////////////////////////////////
extern const NodeDescriptor is_serial;

/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when the node has at least
/// @p n PartEdges (i.e., at least @p n connected sockets).
///
/// @param n Minimum number of edges.
/// @return NodeDescriptor returning true when edges count >= n.
/////////////////////////////////////////////////
NodeDescriptor has_minimum_n_edges(size_t n);

/////////////////////////////////////////////////
/// @brief A NodeDescriptor that returns true when the node has at least 3
/// PartEdges (i.e., at least 3 connected sockets), indicating a "branching"
/// point
/////////////////////////////////////////////////
extern const NodeDescriptor is_branched;

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
} // namespace steamrot::logic::descriptors
