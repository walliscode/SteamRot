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
#include <unordered_map>

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
/// steamrot::tests::AnalysisTraceBuilder builder{id_to_part_graph_id};
/// builder
///   .NodeEval("f0", "is_fragment")
///   .NodeResult("f0", "is_fragment", true, "node holds FragmentInstance");
///
/// steamrot::logic::descriptors::TerminalDescriptorFormatter fmt;
/// REQUIRE_THAT(result.m_trace, steamrot::tests::EqualsTrace(builder.Build(),
/// fmt));
/// @endcode
/////////////////////////////////////////////////
class AnalysisTraceBuilder {
public:
  /////////////////////////////////////////////////
  /// @brief Construct a builder with alias-to-part-id resolution support.
  ///
  /// @param id_to_part_graph_id Mapping from user-friendly string IDs to
  ///                            stable PartGraph IDs.
  /////////////////////////////////////////////////
  explicit AnalysisTraceBuilder(
      const std::unordered_map<std::string, uint32_t> &id_to_part_graph_id);

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
             std::optional<std::string> anchor_id_alias = std::nullopt);

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
  /// @brief Append a NodeEval event using a string part alias.
  ///
  /// @param part_id_alias   User-friendly string alias for the part ID.
  /// @param predicate_name  Name of the predicate applied to the node.
  /// @param depth           Nesting depth of this event.
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &NodeEval(const std::string &part_id_alias,
                                 std::string predicate_name,
                                 uint32_t depth = 0);

  /////////////////////////////////////////////////
  /// @brief Append a NodeResult event using a string part alias.
  ///
  /// @param part_id_alias   User-friendly string alias for the part ID.
  /// @param predicate_name  Name of the predicate.
  /// @param result          Outcome of the predicate evaluation.
  /// @param reason          Human-readable explanation of the outcome.
  /// @param depth           Nesting depth of this event.
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &NodeResult(const std::string &part_id_alias,
                                   std::string predicate_name, bool result,
                                   std::string reason = {}, uint32_t depth = 0);

  /////////////////////////////////////////////////
  /// @brief Append a MovingToNeighbour event using string part aliases.
  ///
  /// @param from_id_alias   Source part alias.
  /// @param from_socket_id  Source socket ID through which the edge is
  ///                        traversed.
  /// @param to_id_alias     Destination part alias.
  /// @param to_socket_id    Destination socket ID reached by the traversal.
  /// @param depth           Nesting depth of this event.
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &MovingToNeighbour(const std::string &from_id_alias,
                                          uint32_t from_socket_id,
                                          const std::string &to_id_alias,
                                          uint32_t to_socket_id,
                                          uint32_t depth);

  /////////////////////////////////////////////////
  /// @brief Append a Backtracking event using a string part alias.
  ///
  /// @param from_id_alias   Part alias of the node being backtracked from.
  /// @param from_socket_id  Socket ID on the source node.
  /// @param to_id_alias     Part alias of the destination node.
  /// @param to_socket_id    Socket ID on the destination node.
  /// @param depth           Nesting depth of this event.
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &Backtracking(const std::string &from_id_alias,
                                     uint32_t from_socket_id,
                                     const std::string &to_id_alias,
                                     uint32_t to_socket_id, uint32_t depth);

  AnalysisTraceBuilder &ValidSubgraphIsolated();

  /////////////////////////////////////////////////
  /// @brief Append a NodeEval event using a raw numeric part ID (no alias).
  ///
  /// Use this overload when the part does not exist in the PartGraph (e.g.
  /// for missing-key error tests), so there is no alias to resolve.
  ///
  /// @param part_id        Raw numeric part ID.
  /// @param predicate_name Name of the predicate applied to the node.
  /// @param depth          Nesting depth of this event.
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &NodeEvalById(uint32_t part_id,
                                     std::string predicate_name,
                                     uint32_t depth = 0);

  /////////////////////////////////////////////////
  /// @brief Append a NodeResult event using a raw numeric part ID (no alias).
  ///
  /// Use this overload when the part does not exist in the PartGraph (e.g.
  /// for missing-key error tests), so there is no alias to resolve.
  ///
  /// @param part_id        Raw numeric part ID.
  /// @param predicate_name Name of the predicate.
  /// @param result         Outcome of the predicate evaluation.
  /// @param reason         Human-readable explanation of the outcome.
  /// @param depth          Nesting depth of this event.
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &NodeResultById(uint32_t part_id,
                                       std::string predicate_name, bool result,
                                       std::string reason = {},
                                       uint32_t depth = 0);

  /////////////////////////////////////////////////
  /// @brief Append an event indicating success in assigning a valid subgraph to
  /// an archetype result field.
  ///
  /// @return Reference to this builder for method chaining.
  /////////////////////////////////////////////////
  AnalysisTraceBuilder &MachinaPartResult(const std::string predicate_name,
                                          const bool result,
                                          uint32_t depth = 0);

  /////////////////////////////////////////////////
  /// @brief Return a copy of the accumulated trace.
  ///
  /// The builder remains usable after this call; further events may be
  /// appended and @c Build() called again.
  /////////////////////////////////////////////////
  steamrot::logic::descriptors::AnalysisTrace Build() const;

private:
  AnalysisTraceBuilder &
  MovingToNeighbourById(uint32_t from_id, uint32_t from_socket_id,
                        uint32_t to_id, uint32_t to_socket_id, uint32_t depth);
  AnalysisTraceBuilder &BacktrackingById(uint32_t from_id,
                                         uint32_t from_socket_id,
                                         uint32_t to_id, uint32_t to_socket_id,
                                         uint32_t depth);
  uint32_t ResolvePartId(const std::string &part_id_alias) const;

  const std::unordered_map<std::string, uint32_t> *m_id_to_part_graph_id{
      nullptr};
  steamrot::logic::descriptors::AnalysisTrace m_trace{};
};

} // namespace steamrot::tests
