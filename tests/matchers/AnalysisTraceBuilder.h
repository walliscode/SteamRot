/////////////////////////////////////////////////
/// @file
/// @brief Fluent builder for constructing AnalysisTrace values in tests.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AnalysisEvent.h"
#include <cstdint>
#include <optional>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class AnalysisTraceBuilder
/// @brief Fluent builder for constructing @c AnalysisTrace values in tests.
///
/// Provides one named method per @c AnalysisEvent kind so that expected
/// traces can be composed programmatically rather than constructed from
/// raw strings.  Pass the result to @c TraceEqualsMatcher together with a
/// @c DescriptorFormatter to verify formatted descriptor output without
/// ever writing a hardcoded expected string.
///
/// @c Build() returns a copy of the accumulated trace, so the builder may
/// be discarded after the call.
///
/// Example:
/// @code
/// steamrot::tests::AnalysisTraceBuilder builder;
/// builder
///   .NodeEval(frag_id, "is_fragment")
///   .NodeResult(frag_id, "is_fragment", true, "node holds FragmentInstance");
///
/// steamrot::logic::descriptors::TerminalDescriptorFormatter fmt;
/// REQUIRE_THAT(result.m_trace, steamrot::tests::EqualsTrace(builder.Build(),
/// fmt));
/// @endcode
/////////////////////////////////////////////////
class AnalysisTraceBuilder {
public:
  /////////////////////////////////////////////////
  /// @brief Append an EmptyPartGraph event.
  ///
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &EmptyPartGraph();

  /////////////////////////////////////////////////
  /// @brief Append a ScopeBegin event.
  ///
  /// @param name      Name of the descriptor scope.
  /// @param kind      Level of the scope (Node, Chain, or Graph).
  /// @param depth     Nesting depth of this event.
  /// @param anchor_id Optional anchor part ID (populated for Chain scopes).
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &
  ScopeBegin(std::string name, steamrot::logic::descriptors::ScopeKind kind,
             uint32_t depth = 0,
             std::optional<uint32_t> anchor_id = std::nullopt);

  /////////////////////////////////////////////////
  /// @brief Append a ScopeEnd event.
  ///
  /// @param name   Name of the descriptor scope.
  /// @param kind   Level of the scope (Node, Chain, or Graph).
  /// @param result Outcome of the scope evaluation.
  /// @param depth  Nesting depth of this event.
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &ScopeEnd(std::string name,
                                 steamrot::logic::descriptors::ScopeKind kind,
                                 bool result, uint32_t depth);

  /////////////////////////////////////////////////
  /// @brief Append a NodeEval event.
  ///
  /// @param part_id        Stable part ID of the node being evaluated.
  /// @param predicate_name Name of the predicate applied to the node.
  /// @param depth          Nesting depth of this event.
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &NodeEval(uint32_t part_id, std::string predicate_name,
                                 uint32_t depth = 0);

  /////////////////////////////////////////////////
  /// @brief Append a NodeResult event.
  ///
  /// @param part_id        Stable part ID of the node evaluated.
  /// @param predicate_name Name of the predicate.
  /// @param result         Outcome of the predicate evaluation.
  /// @param reason         Human-readable explanation of the outcome.
  /// @param depth          Nesting depth of this event.
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &NodeResult(uint32_t part_id, std::string predicate_name,
                                   bool result, std::string reason = {},
                                   uint32_t depth = 0);

  /////////////////////////////////////////////////
  /// @brief Append a MovingToNeighbour event.
  ///
  /// @param from_id   Part ID of the source node.
  /// @param to_id     Part ID of the destination node.
  /// @param socket_id Socket ID on the source part through which the edge is
  ///                  traversed.
  /// @param depth     Nesting depth of this event.
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &MovingToNeighbour(uint32_t from_id, uint32_t to_id,
                                          uint32_t socket_id, uint32_t depth);

  /////////////////////////////////////////////////
  /// @brief Append a Backtracking event.
  ///
  /// @param from_id Part ID of the node being backtracked from.
  /// @param depth   Nesting depth of this event.
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &Backtracking(uint32_t from_id, uint32_t depth);

  AnalysisTraceBuilder &ValidSubgraphIsolated();

  /////////////////////////////////////////////////
  /// @brief Return a copy of the accumulated trace.
  ///
  /// The builder remains usable after this call; further events may be
  /// appended and @c Build() called again.
  /////////////////////////////////////////////////
  steamrot::logic::descriptors::AnalysisTrace Build() const;

private:
  steamrot::logic::descriptors::AnalysisTrace m_trace{};
};

} // namespace steamrot::tests
