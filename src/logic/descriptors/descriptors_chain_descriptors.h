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
#include "NodeDescriptor.h"

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @brief Lift a NodeDescriptor to a ChainDescriptor.
///
/// Wraps @p nd so that its @c NodeDescriptorResult is promoted to a
/// @c ChainDescriptorResult. The node trace events produced by @p nd are
/// forwarded into the chain result's @c m_trace. No graph walk is
/// performed; the anchor node is the only node evaluated.
///
/// @param nd NodeDescriptor to lift.
/// @return ChainDescriptor that delegates to @p nd.
/////////////////////////////////////////////////
inline ChainDescriptor lift_to_chain(NodeDescriptor nd) {
  return ChainDescriptor{
      nd.GetName(),
      [nd = std::move(nd)](const PartGraph &parts, uint32_t id,
                           uint32_t depth) -> ChainDescriptorResult {
        auto node_result = nd(parts, id, depth);
        ChainDescriptorResult result{static_cast<bool>(node_result)};
        Merge(result.m_trace, std::move(node_result.m_trace));
        return result;
      }};
}

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
extern const ChainDescriptor is_serial_chain_with_minimum_length_2;
} // namespace steamrot::logic::descriptors
