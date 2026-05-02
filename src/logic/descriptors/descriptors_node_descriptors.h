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
#include "MachinaFormScaffold.h"
#include <cstddef>
#include <cstdint>
#include <functional>
namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @brief Predicate type for single-node queries on a MachinaFormScaffold.
///
/// Any callable with signature
/// @c NodeDescriptorResult(const MachinaFormScaffold&, uint32_t part_id)
/// qualifies. @c is_fragment, @c is_joint, and the edge-count helpers are
/// declared as @c const @c NodeDescriptor variables and can be used directly
/// or assigned to other @c NodeDescriptor instances.
///
/// Example:
/// @code
/// NodeDescriptor predicate = descriptors::is_fragment;
/// NodeDescriptorResult result = predicate(scaffold, id);
/// @endcode
/////////////////////////////////////////////////
using NodeDescriptor =
    std::function<NodeDescriptorResult(const MachinaFormScaffold &, uint32_t)>;

/////////////////////////////////////////////////
/// @brief Predicate for a single node with full access to the scaffold.
///
/// Any callable with signature
/// @c NodeDescriptorResult(const MachinaFormScaffold&, uint32_t part_id)
/// qualifies. Semantically distinct from @c NodeDescriptor: use
/// @c ContextualNodeDescriptor when the predicate examines neighbouring
/// nodes via socket traversal, and @c NodeDescriptor when it examines only
/// the named node's own data.
/////////////////////////////////////////////////
using ContextualNodeDescriptor =
    std::function<NodeDescriptorResult(const MachinaFormScaffold &, uint32_t)>;

/////////////////////////////////////////////////
/// @brief Lift a NodeDescriptor to a ContextualNodeDescriptor.
///
/// Both types share the same signature; this is an identity conversion
/// used to make the semantic promotion explicit at call sites.
///
/// @param nd NodeDescriptor to lift.
/// @return ContextualNodeDescriptor wrapping @p nd.
/////////////////////////////////////////////////
inline ContextualNodeDescriptor lift(NodeDescriptor nd) { return nd; }
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
/// @p n connections (i.e., exactly @p n connected sockets).
///
/// @param n Number of connections.
/// @return NodeDescriptor returning true when connection_count == n.
/////////////////////////////////////////////////
NodeDescriptor has_exactly_n_edges(size_t n);

/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when the node has exactly
/// 2 connections.
///
/// serial is used to indicate part of a serial chain
/////////////////////////////////////////////////
extern const NodeDescriptor is_serial;

/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when the node has at least
/// @p n connections (i.e., at least @p n connected sockets).
///
/// @param n Minimum number of connections.
/// @return NodeDescriptor returning true when connection_count >= n.
/////////////////////////////////////////////////
NodeDescriptor has_minimum_n_edges(size_t n);

/////////////////////////////////////////////////
/// @brief A NodeDescriptor that returns true when the node has at least 3
/// connections (i.e., at least 3 connected sockets), indicating a "branching"
/// point
/////////////////////////////////////////////////
extern const NodeDescriptor is_branched;

/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when the node has at
/// most @p n connections (i.e., at most @p n connected sockets).
///
/// @param n Maximum number of connections.
/// @return NodeDescriptor returning true when connection_count <= n.
/////////////////////////////////////////////////
NodeDescriptor has_maximum_n_edges(size_t n);

/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true for nodes with at most 1 connected
/// socket (i.e., at the "end" of a chain of connections).
/////////////////////////////////////////////////
extern const NodeDescriptor is_terminal;
} // namespace steamrot::logic::descriptors
