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
#include "NodeDescriptor.h"
#include <cstddef>
#include <string>

namespace steamrot::logic::descriptors {

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
/// @param name Human-readable name for the descriptor, used in trace events.
/// @return NodeDescriptor returning true when connection_count == n.
/////////////////////////////////////////////////
NodeDescriptor has_exactly_n_edges(size_t n, std::string name);

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
