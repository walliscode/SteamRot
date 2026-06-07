/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ChainDescriptor class
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
#include <cstdint>
#include <functional>
#include <unordered_set>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @class ChainDescriptor
/// @brief Named, callable predicate for a multi-hop walk from one anchor node.
///
/// Wraps a @c std::function with the signature
/// @c ChainDescriptorResult(const PartGraph&, uint32_t part_id).
/// A @c ChainDescriptor answers a structural question about a subgraph
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
  using FnType = std::function<ChainDescriptorResult(
      const PartGraph &, uint32_t, std::unordered_set<uint32_t>, uint32_t)>;

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
  /// @param visited copy of Set of part IDs already visited, this could be from
  /// a MachinaArchetype
  /// @param depth Nesting depth used for chain-scope trace events.
  /////////////////////////////////////////////////
  ChainDescriptorResult operator()(const PartGraph &parts, uint32_t id,
                                   std::unordered_set<uint32_t> visited = {},
                                   uint32_t depth = 0) const {
    return m_fn(parts, id, visited, depth);
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
} // namespace steamrot::logic::descriptors
