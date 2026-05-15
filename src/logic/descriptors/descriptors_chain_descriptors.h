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
#include <string>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @class ChainDescriptor
/// @brief Named, callable predicate for a multi-hop walk from one anchor node.
///
/// Wraps a @c std::function with the signature
/// @c ChainDescriptorResult(const PartGraph&, uint32_t part_id).
/// The distinction from @c ContextualNodeDescriptor is semantic:
/// a @c ChainDescriptor answers a structural question about a subgraph
/// reachable from the start node (e.g. "does a 3-node linear chain begin
/// here?") rather than a question about the start node alone.
///
/// Build instances with @c steamrot::logic::descriptors::ChainDescriptorBuilder
/// (see @c ChainDescriptorBuilder.h).
/////////////////////////////////////////////////
class ChainDescriptor {
public:
  /////////////////////////////////////////////////
  /// @brief Underlying callable type.
  /////////////////////////////////////////////////
  using FnType =
      std::function<ChainDescriptorResult(const PartGraph &, uint32_t)>;

  ChainDescriptor() = default;

  /////////////////////////////////////////////////
  /// @brief Construct a named descriptor.
  ///
  /// @param name Human-readable descriptor name used in trace events.
  /// @param fn   Callable implementing the chain walk.
  /////////////////////////////////////////////////
  ChainDescriptor(std::string name, FnType fn)
      : m_name(std::move(name)), m_fn(std::move(fn)) {}

  /////////////////////////////////////////////////
  /// @brief Evaluate the chain descriptor from @p id as anchor.
  ///
  /// The returned @c ChainDescriptorResult already has its @c m_trace
  /// populated by the DFS (via ScopeBegin/ScopeEnd and per-node events).
  ///
  /// @param parts The PartGraph to query.
  /// @param id    Stable part ID of the anchor node.
  /////////////////////////////////////////////////
  ChainDescriptorResult operator()(const PartGraph &parts, uint32_t id) const {
    return m_fn(parts, id);
  }

  /////////////////////////////////////////////////
  /// @brief Return the descriptor's stable name.
  /////////////////////////////////////////////////
  const std::string &GetName() const noexcept { return m_name; }

  /////////////////////////////////////////////////
  /// @brief Return the underlying std::function.
  /////////////////////////////////////////////////
  const FnType &GetFn() const noexcept { return m_fn; }

  /////////////////////////////////////////////////
  /// @brief True when the descriptor holds a callable.
  /////////////////////////////////////////////////
  explicit operator bool() const noexcept { return static_cast<bool>(m_fn); }

private:
  std::string m_name{};
  FnType m_fn{};
};

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
      [nd = std::move(nd)](const PartGraph &parts,
                           uint32_t id) -> ChainDescriptorResult {
        auto node_result = nd(parts, id);
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

} // namespace steamrot::logic::descriptors
