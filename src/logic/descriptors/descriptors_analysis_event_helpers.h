/////////////////////////////////////////////////
/// @file
/// @brief Helper functions for constructing and appending descriptor
///        analysis events.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AnalysisEvent.h"
#include "MachinaFormScaffold.h"
#include <cstdint>
#include <optional>
#include <string>
#include <utility>

namespace steamrot::logic::descriptors {

namespace detail {
inline std::string resolve_part_alias(const PartGraph &parts, uint32_t id) {
  if (const auto part_it = parts.find(id); part_it != parts.end()) {
    return std::visit(
        [](const auto &inst) -> const std::string & { return inst.alias; },
        part_it->second);
  }
  return {};
}
} // namespace detail

inline void append_event(AnalysisTrace &trace, AnalysisEvent event) {
  trace.push_back(std::move(event));
}

template <typename Context>
concept HasAnalysisTrace = requires(Context &ctx) { ctx.trace; };

template <HasAnalysisTrace Context>
inline void append_event(Context &context, AnalysisEvent event) {
  context.trace.push_back(std::move(event));
}

inline AnalysisEvent make_empty_part_graph_event() {
  AnalysisEvent event{};
  event.kind = TraceEventKind::EmtpyPartGraph;
  return event;
}

inline AnalysisEvent make_empty_chain_steps_event() {
  AnalysisEvent event{};
  event.kind = TraceEventKind::EmtpyChainSteps;
  return event;
}

inline AnalysisEvent make_invalid_subgraph_isolated_event(uint32_t depth) {
  AnalysisEvent event{};
  event.kind = TraceEventKind::InvalidSubgraphIsolated;
  event.depth = depth;
  return event;
}

inline AnalysisEvent
make_scope_begin_event(std::string scope_name, ScopeKind scope_kind,
                       uint32_t depth = 0,
                       std::optional<uint32_t> anchor_id = std::nullopt) {
  AnalysisEvent event{};
  event.kind = TraceEventKind::ScopeBegin;
  event.depth = depth;
  event.scope_name = std::move(scope_name);
  event.scope_kind = scope_kind;
  event.anchor_id = anchor_id;
  return event;
}

inline AnalysisEvent
make_scope_begin_event(std::string scope_name, ScopeKind scope_kind,
                       const PartGraph &parts, uint32_t depth = 0,
                       std::optional<uint32_t> anchor_id = std::nullopt) {
  AnalysisEvent event = make_scope_begin_event(std::move(scope_name),
                                               scope_kind, depth, anchor_id);
  if (anchor_id.has_value())
    event.part_id_alias = detail::resolve_part_alias(parts, *anchor_id);
  return event;
}

inline AnalysisEvent make_scope_end_event(std::string scope_name,
                                          ScopeKind scope_kind, bool result,
                                          uint32_t depth = 0) {
  AnalysisEvent event{};
  event.kind = TraceEventKind::ScopeEnd;
  event.depth = depth;
  event.scope_name = std::move(scope_name);
  event.scope_kind = scope_kind;
  event.result = result;
  return event;
}

inline AnalysisEvent make_node_eval_event(uint32_t depth, uint32_t part_id,
                                          std::string predicate_name) {
  AnalysisEvent event{};
  event.kind = TraceEventKind::NodeEval;
  event.depth = depth;
  event.part_id = part_id;
  event.predicate_name = std::move(predicate_name);
  return event;
}

inline AnalysisEvent make_node_eval_event(uint32_t depth, uint32_t part_id,
                                          std::string predicate_name,
                                          const PartGraph &parts) {
  AnalysisEvent event =
      make_node_eval_event(depth, part_id, std::move(predicate_name));
  event.part_id_alias = detail::resolve_part_alias(parts, part_id);
  return event;
}

inline AnalysisEvent make_node_result_event(uint32_t depth, uint32_t part_id,
                                            std::string predicate_name,
                                            bool result,
                                            std::string reason = {},
                                            std::string part_id_alias = {}) {
  AnalysisEvent event{};
  event.kind = TraceEventKind::NodeResult;
  event.depth = depth;
  event.part_id = part_id;
  event.part_id_alias = std::move(part_id_alias);
  event.predicate_name = std::move(predicate_name);
  event.result = result;
  event.reason = std::move(reason);
  return event;
}

inline AnalysisEvent make_moving_to_neighbour_event(
    uint32_t depth, uint32_t from_id, uint32_t from_socket_id, uint32_t to_id,
    uint32_t to_socket_id, std::string from_id_alias = {},
    std::string to_id_alias = {}) {
  AnalysisEvent event{};
  event.kind = TraceEventKind::MovingToNeighbour;
  event.depth = depth;
  event.from_id = from_id;
  event.from_id_alias = std::move(from_id_alias);
  event.from_socket_id = from_socket_id;
  event.to_id = to_id;
  event.to_id_alias = std::move(to_id_alias);
  event.to_socket_id = to_socket_id;
  return event;
}

inline AnalysisEvent
make_machina_part_result_event(const std::string predicate_name,
                               const bool result, uint32_t depth) {
  AnalysisEvent event{};
  event.kind = TraceEventKind::MachinaPartResult;
  event.depth = depth;
  event.predicate_name = std::move(predicate_name);
  event.result = result;
  return event;
}

inline AnalysisEvent
make_moving_to_neighbour_event(uint32_t depth, uint32_t from_id,
                               uint32_t from_socket_id, uint32_t to_id,
                               uint32_t to_socket_id, const PartGraph &parts) {
  return make_moving_to_neighbour_event(
      depth, from_id, from_socket_id, to_id, to_socket_id,
      detail::resolve_part_alias(parts, from_id),
      detail::resolve_part_alias(parts, to_id));
}

inline AnalysisEvent make_backtracking_event(uint32_t depth, uint32_t from_id,
                                             uint32_t from_socket_id,
                                             uint32_t to_id,
                                             uint32_t to_socket_id,
                                             std::string from_id_alias = {},
                                             std::string to_id_alias = {}) {
  AnalysisEvent event{};
  event.kind = TraceEventKind::Backtracking;
  event.depth = depth;
  event.from_id = from_id;
  event.from_id_alias = std::move(from_id_alias);
  event.from_socket_id = from_socket_id;
  event.to_id = to_id;
  event.to_id_alias = std::move(to_id_alias);
  event.to_socket_id = to_socket_id;
  return event;
}

inline AnalysisEvent make_backtracking_event(uint32_t depth, uint32_t from_id,
                                             uint32_t from_socket_id,
                                             uint32_t to_id,
                                             uint32_t to_socket_id,
                                             const PartGraph &parts) {
  return make_backtracking_event(depth, from_id, from_socket_id, to_id,
                                 to_socket_id,
                                 detail::resolve_part_alias(parts, from_id),
                                 detail::resolve_part_alias(parts, to_id));
}

template <typename Context>
inline void add_empty_part_graph_event(Context &context) {
  append_event(context, make_empty_part_graph_event());
}

template <typename Context>
inline void add_empty_chain_steps_event(Context &context) {
  append_event(context, make_empty_chain_steps_event());
}

template <typename Context>
inline void add_invalid_subgraph_isolated_event(Context &context,
                                                uint32_t depth) {
  append_event(context, make_invalid_subgraph_isolated_event(depth));
}

template <typename Context>
inline void
add_scope_begin_event(Context &context, std::string scope_name,
                      ScopeKind scope_kind, uint32_t depth = 0,
                      std::optional<uint32_t> anchor_id = std::nullopt) {
  append_event(context, make_scope_begin_event(std::move(scope_name),
                                               scope_kind, depth, anchor_id));
}

template <typename Context>
inline void
add_scope_begin_event(Context &context, std::string scope_name,
                      ScopeKind scope_kind, const PartGraph &parts,
                      uint32_t depth = 0,
                      std::optional<uint32_t> anchor_id = std::nullopt) {
  append_event(context,
               make_scope_begin_event(std::move(scope_name), scope_kind, parts,
                                      depth, anchor_id));
}

template <typename Context>
inline void add_scope_end_event(Context &context, std::string scope_name,
                                ScopeKind scope_kind, bool result,
                                uint32_t depth = 0) {
  append_event(context, make_scope_end_event(std::move(scope_name), scope_kind,
                                             result, depth));
}

} // namespace steamrot::logic::descriptors
