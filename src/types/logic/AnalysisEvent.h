/////////////////////////////////////////////////
/// @file
/// @brief Event types emitted during descriptor evaluations, forming a
///        structured analysis trace.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @enum ScopeKind
/// @brief Identifies the descriptor level that opened a trace scope.
/////////////////////////////////////////////////
enum class ScopeKind {
  Node,
  Chain,
  MachinaArchetype,
};

/////////////////////////////////////////////////
/// @enum TraceEventKind
/// @brief Identifies the kind of analysis event recorded in a trace.
/////////////////////////////////////////////////
enum class TraceEventKind {
  EmtpyPartGraph,
  EmtpyChainSteps,
  NodeEval,
  MovingToNeighbour,
  Backtracking,
  ScopeBegin,
  ScopeEnd,
  MachinaPartResult,
  ValidSubgraphIsolated,
  InvalidSubgraphIsolated,
};

/////////////////////////////////////////////////
/// @struct AnalysisEvent
/// @brief A single structured event in a descriptor analysis trace.
///
/// Not all fields are relevant for every event kind. Consult the @c kind
/// field to determine which fields are populated for a given event.
///
/// | kind               | fields used |
/// |--------------------|------------------------------------------------------|
/// | NodeEval           | depth, part_id, predicate_name |
/// | NodeResult         | depth, part_id, predicate_name, result, reason |
/// | MovingToNeighbour  | depth, from_id, to_id, from_socket_id, to_socket_id |
/// | Backtracking       | depth, from_id, to_id, from_socket_id, to_socket_id |
/// | ScopeBegin         | depth, scope_name, scope_kind, anchor_id (optional) |
/// | ScopeEnd           | depth, scope_name, scope_kind, result |
/////////////////////////////////////////////////
struct AnalysisEvent {
  /////////////////////////////////////////////////
  /// @brief The kind of event.
  /////////////////////////////////////////////////
  TraceEventKind kind{TraceEventKind::NodeEval};

  /////////////////////////////////////////////////
  /// @brief Nesting depth at which this event was emitted.
  ///
  /// Depth 0 = chain/graph scope boundary (ScopeBegin/ScopeEnd).
  /// Depth 1 = anchor node evaluation. Depth 2+ = nodes reached by DFS.
  /////////////////////////////////////////////////
  uint32_t depth{0};

  // ── NodeEval / NodeResult ─────────────────────────────────────────────────

  /////////////////////////////////////////////////
  /// @brief Stable part ID of the node being evaluated.
  /// Populated for NodeEval and NodeResult.
  /////////////////////////////////////////////////
  uint32_t part_id{0};

  /////////////////////////////////////////////////
  /// @brief Human readable alias for the part ID, if available.
  /////////////////////////////////////////////////
  std::string part_id_alias{};

  /////////////////////////////////////////////////
  /// @brief Name of the predicate applied to the node.
  /// Populated for NodeEval and NodeResult.
  /////////////////////////////////////////////////
  std::string predicate_name{};

  // ── NodeResult / ScopeEnd ─────────────────────────────────────────────────

  /////////////////////////////////////////////////
  /// @brief Outcome of the predicate or scope evaluation.
  /// Populated for NodeResult and ScopeEnd.
  /////////////////////////////////////////////////
  bool result{false};

  /////////////////////////////////////////////////
  /// @brief Human-readable explanation of the predicate outcome.
  /// Populated for NodeResult.
  /////////////////////////////////////////////////
  std::string reason{};

  // ── MovingToNeighbour / Backtracking ──────────────────────────────────────

  /////////////////////////////////////////////////
  /// @brief Part ID of the source node for a traversal or backtrack.
  /// Populated for MovingToNeighbour and Backtracking.
  /////////////////////////////////////////////////
  uint32_t from_id{0};

  /////////////////////////////////////////////////
  /// @brief Human readable alias for the source part ID, if available.
  /////////////////////////////////////////////////
  std::string from_id_alias{};

  /////////////////////////////////////////////////
  /// @brief Part ID of the destination node for a traversal or backtrack.
  /// Populated for MovingToNeighbour and Backtracking.
  /////////////////////////////////////////////////
  uint32_t to_id{0};

  /////////////////////////////////////////////////
  /// @brief Human readable alias for the destination part ID, if available.
  /////////////////////////////////////////////////
  std::string to_id_alias{};

  /////////////////////////////////////////////////
  /// @brief Socket ID on the source node through which the edge is traversed.
  /// Populated for MovingToNeighbour and Backtracking.
  /////////////////////////////////////////////////
  uint32_t from_socket_id{0};

  /////////////////////////////////////////////////
  /// @brief Socket ID on the destination node reached by the traversal.
  /// Populated for MovingToNeighbour and Backtracking.
  /////////////////////////////////////////////////
  uint32_t to_socket_id{0};

  // ── ScopeBegin / ScopeEnd ─────────────────────────────────────────────────

  /////////////////////////////////////////////////
  /// @brief Name of the descriptor scope (e.g. the chain's name).
  /// Populated for ScopeBegin and ScopeEnd.
  /////////////////////////////////////////////////
  std::string scope_name{};

  /////////////////////////////////////////////////
  /// @brief Level of the descriptor scope.
  /// Populated for ScopeBegin and ScopeEnd.
  /////////////////////////////////////////////////
  ScopeKind scope_kind{ScopeKind::Node};

  /////////////////////////////////////////////////
  /// @brief Part ID of the anchor node, when applicable.
  /// Populated for ScopeBegin with ScopeKind::Chain.
  /////////////////////////////////////////////////
  std::optional<uint32_t> anchor_id{std::nullopt};
};

/////////////////////////////////////////////////
/// @brief A flat, ordered sequence of analysis events produced during one
///        descriptor evaluation.
/////////////////////////////////////////////////
using AnalysisTrace = std::vector<AnalysisEvent>;

/////////////////////////////////////////////////
/// @brief Append all events from @p source into @p target.
///
/// Moves every event out of @p source. After the call @p source is empty.
///
/// @param target Destination trace.
/// @param source Source trace (consumed).
/////////////////////////////////////////////////
inline void Merge(AnalysisTrace &target, AnalysisTrace &&source) {
  target.insert(target.end(), std::make_move_iterator(source.begin()),
                std::make_move_iterator(source.end()));
  source.clear();
}

} // namespace steamrot::logic::descriptors
