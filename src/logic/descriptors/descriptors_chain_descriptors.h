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
#include "ChainDescriptor.h"

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @brief Checks whether a serial chain can be formed from the provided node
///
/// A serial chain is that in which the node has exactly 2 connections and then
/// finally a terminal node
/////////////////////////////////////////////////
extern const ChainDescriptor is_serial_chain;

/////////////////////////////////////////////////
/// @brief Returns a ChainDescriptor that checks whether a serial chain of at
/// least @p n
///
/// @param n Minimum length of the serial chain, including the anchor node.
/// @return ChainDescriptor object
/////////////////////////////////////////////////
ChainDescriptor is_serial_chain_with_minimum_length_n(uint32_t n);

/////////////////////////////////////////////////
/// @brief ChainDescriptor that checks whether a serial chain of at least 2
/// nodes
/////////////////////////////////////////////////
extern const ChainDescriptor is_serial_chain_with_minimum_length_3;

/////////////////////////////////////////////////
/// @brief ChainDescriptor that checks whether a node is a JointInstance
///
/// Wraps the @p is_joint NodeDescriptor in a chain scope, emitting
/// ScopeBegin/ScopeEnd trace events for consistent trace output.
/////////////////////////////////////////////////
extern const ChainDescriptor is_joint_chain;
} // namespace steamrot::logic::descriptors
