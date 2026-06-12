/////////////////////////////////////////////////
/// @file
/// @brief Implementation of AnalysisTraceBuilder.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AnalysisTraceBuilder.h"
#include <stdexcept>

namespace steamrot::tests {

/////////////////////////////////////////////////
AnalysisTraceBuilder::AnalysisTraceBuilder(
    const std::unordered_map<std::string, uint32_t> &id_to_part_graph_id)
    : m_id_to_part_graph_id{&id_to_part_graph_id} {}

/////////////////////////////////////////////////
AnalysisTraceBuilder &AnalysisTraceBuilder::EmptyPartGraph() {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::EmtpyPartGraph;
  m_trace.push_back(std::move(ev));
  return *this;
}
/////////////////////////////////////////////////
AnalysisTraceBuilder &
AnalysisTraceBuilder::ScopeBegin(std::string name, ScopeKind kind,
                                 uint32_t depth,
                                 std::optional<std::string> anchor_id_alias) {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::ScopeBegin;
  ev.depth = depth;
  ev.scope_name = std::move(name);
  ev.scope_kind = kind;
  if (anchor_id_alias.has_value()) {
    ev.anchor_id = ResolvePartId(anchor_id_alias.value());
    ev.part_id_alias = anchor_id_alias.value();
  }
  m_trace.push_back(std::move(ev));
  return *this;
}

/////////////////////////////////////////////////
AnalysisTraceBuilder &AnalysisTraceBuilder::ScopeEnd(std::string name,
                                                     ScopeKind kind,
                                                     bool result,
                                                     uint32_t depth) {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::ScopeEnd;
  ev.depth = depth;
  ev.scope_name = std::move(name);
  ev.scope_kind = kind;
  ev.result = result;
  m_trace.push_back(std::move(ev));
  return *this;
}

/////////////////////////////////////////////////
AnalysisTraceBuilder &
AnalysisTraceBuilder::NodeEvalById(uint32_t part_id, std::string predicate_name,
                                   const bool result, uint32_t depth,
                                   std::string reason) {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::NodeEval;
  ev.depth = depth;
  ev.part_id = part_id;
  ev.predicate_name = std::move(predicate_name);
  ev.result = result;
  ev.reason = std::move(reason);
  m_trace.push_back(std::move(ev));
  return *this;
}

/////////////////////////////////////////////////
AnalysisTraceBuilder &
AnalysisTraceBuilder::NodeEval(const std::string &part_id_alias,
                               std::string predicate_name, const bool result,
                               uint32_t depth, std::string reason) {

  AnalysisEvent ev{};
  ev.kind = TraceEventKind::NodeEval;
  ev.depth = depth;
  ev.part_id = ResolvePartId(part_id_alias);
  ev.part_id_alias = part_id_alias;
  ev.predicate_name = std::move(predicate_name);
  ev.result = result;
  ev.reason = std::move(reason);
  m_trace.push_back(std::move(ev));
  return *this;
}

/////////////////////////////////////////////////
AnalysisTraceBuilder &AnalysisTraceBuilder::MovingToNeighbourById(
    uint32_t from_id, uint32_t from_socket_id, uint32_t to_id,
    uint32_t to_socket_id, uint32_t depth) {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::MovingToNeighbour;
  ev.depth = depth;
  ev.from_id = from_id;
  ev.from_socket_id = from_socket_id;
  ev.to_id = to_id;
  ev.to_socket_id = to_socket_id;
  m_trace.push_back(std::move(ev));
  return *this;
}

/////////////////////////////////////////////////
AnalysisTraceBuilder &AnalysisTraceBuilder::MovingToNeighbour(
    const std::string &from_id_alias, uint32_t from_socket_id,
    const std::string &to_id_alias, uint32_t to_socket_id, uint32_t depth) {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::MovingToNeighbour;
  ev.depth = depth;
  ev.from_id = ResolvePartId(from_id_alias);
  ev.from_id_alias = from_id_alias;
  ev.from_socket_id = from_socket_id;
  ev.to_id = ResolvePartId(to_id_alias);
  ev.to_id_alias = to_id_alias;
  ev.to_socket_id = to_socket_id;
  m_trace.push_back(std::move(ev));
  return *this;
}

/////////////////////////////////////////////////
AnalysisTraceBuilder &
AnalysisTraceBuilder::BacktrackingById(uint32_t from_id,
                                       uint32_t from_socket_id, uint32_t to_id,
                                       uint32_t to_socket_id, uint32_t depth) {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::Backtracking;
  ev.depth = depth;
  ev.from_id = from_id;
  ev.from_socket_id = from_socket_id;
  ev.to_id = to_id;
  ev.to_socket_id = to_socket_id;
  m_trace.push_back(std::move(ev));
  return *this;
}
/////////////////////////////////////////////////
AnalysisTraceBuilder &AnalysisTraceBuilder::Backtracking(
    const std::string &from_id_alias, uint32_t from_socket_id,
    const std::string &to_id_alias, uint32_t to_socket_id, uint32_t depth) {

  AnalysisEvent ev{};
  ev.kind = TraceEventKind::Backtracking;
  ev.depth = depth;
  ev.from_id = ResolvePartId(from_id_alias);
  ev.from_id_alias = from_id_alias;
  ev.from_socket_id = from_socket_id;
  ev.to_id = ResolvePartId(to_id_alias);
  ev.to_id_alias = to_id_alias;
  ev.to_socket_id = to_socket_id;
  m_trace.push_back(std::move(ev));
  return *this;
}
/////////////////////////////////////////////////
AnalysisTraceBuilder &AnalysisTraceBuilder::ValidSubgraphIsolated() {

  return *this;
}

/////////////////////////////////////////////////
AnalysisTraceBuilder &
AnalysisTraceBuilder::MachinaPartResult(const std::string predicate_name,
                                        const bool result, uint32_t depth) {

  AnalysisEvent ev{};
  ev.kind = TraceEventKind::MachinaPartResult;
  ev.depth = depth;
  ev.predicate_name = std::move(predicate_name);
  ev.result = result;
  m_trace.push_back(std::move(ev));

  return *this;
}

/////////////////////////////////////////////////
AnalysisTrace AnalysisTraceBuilder::Build() const { return m_trace; }

/////////////////////////////////////////////////
uint32_t
AnalysisTraceBuilder::ResolvePartId(const std::string &part_id_alias) const {
  if (!m_id_to_part_graph_id) {
    throw std::logic_error(
        "AnalysisTraceBuilder requires id_to_part_graph_id for string IDs");
  }

  const auto it = m_id_to_part_graph_id->find(part_id_alias);
  if (it == m_id_to_part_graph_id->end())
    throw std::out_of_range("Unknown part ID alias: " + part_id_alias);

  return it->second;
}

} // namespace steamrot::tests
