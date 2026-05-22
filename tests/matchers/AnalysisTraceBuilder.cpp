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

using namespace steamrot::logic::descriptors;

/////////////////////////////////////////////////
AnalysisTraceBuilder &AnalysisTraceBuilder::EmptyPartGraph() {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::EmtpyPartGraph;
  m_trace.push_back(std::move(ev));
  return *this;
}
/////////////////////////////////////////////////
///////////////////////////////////////////////
AnalysisTraceBuilder &AnalysisTraceBuilder::BindAliases(
    const steamrot::tests::ScaffoldResult &scaffold_result) {
  m_alias_to_id = &scaffold_result.alias_to_id;
  return *this;
}

///////////////////////////////////////////////
AnalysisTraceBuilder &
AnalysisTraceBuilder::ScopeBegin(std::string name, ScopeKind kind,
                                 uint32_t depth,
                                 std::optional<uint32_t> anchor_id) {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::ScopeBegin;
  ev.depth = depth;
  ev.scope_name = std::move(name);
  ev.scope_kind = kind;
  ev.anchor_id = anchor_id;
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
AnalysisTraceBuilder::ScopeBeginNamed(std::string name, ScopeKind kind,
                                      uint32_t depth,
                                      std::optional<std::string> anchor_alias) {
  if (!anchor_alias.has_value())
    return ScopeBegin(std::move(name), kind, depth, std::nullopt);
  return ScopeBegin(std::move(name), kind, depth,
                    ResolveAlias(anchor_alias.value()));
}

///////////////////////////////////////////////
AnalysisTraceBuilder &AnalysisTraceBuilder::NodeEval(uint32_t part_id,
                                                     std::string predicate_name,
                                                     uint32_t depth) {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::NodeEval;
  ev.depth = depth;
  ev.part_id = part_id;
  ev.predicate_name = std::move(predicate_name);
  m_trace.push_back(std::move(ev));
  return *this;
}

/////////////////////////////////////////////////
AnalysisTraceBuilder &AnalysisTraceBuilder::NodeEvalNamed(
    std::string alias, std::string predicate_name, uint32_t depth) {
  return NodeEval(ResolveAlias(alias), std::move(predicate_name), depth);
}

///////////////////////////////////////////////
AnalysisTraceBuilder &
AnalysisTraceBuilder::NodeResult(uint32_t part_id, std::string predicate_name,
                                 bool result, std::string reason,
                                 uint32_t depth) {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::NodeResult;
  ev.depth = depth;
  ev.part_id = part_id;
  ev.predicate_name = std::move(predicate_name);
  ev.result = result;
  ev.reason = std::move(reason);
  m_trace.push_back(std::move(ev));
  return *this;
}

/////////////////////////////////////////////////
AnalysisTraceBuilder &AnalysisTraceBuilder::NodeResultNamed(
    std::string alias, std::string predicate_name, bool result,
    std::string reason, uint32_t depth) {
  return NodeResult(ResolveAlias(alias), std::move(predicate_name), result,
                    std::move(reason), depth);
}

///////////////////////////////////////////////
AnalysisTraceBuilder &
AnalysisTraceBuilder::MovingToNeighbour(uint32_t from_id, uint32_t to_id,
                                        uint32_t socket_id, uint32_t depth) {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::MovingToNeighbour;
  ev.depth = depth;
  ev.from_id = from_id;
  ev.to_id = to_id;
  ev.socket_id = socket_id;
  m_trace.push_back(std::move(ev));
  return *this;
}

/////////////////////////////////////////////////
AnalysisTraceBuilder &
AnalysisTraceBuilder::MovingToNeighbourNamed(std::string from_alias,
                                             std::string to_alias,
                                             uint32_t socket_id,
                                             uint32_t depth) {
  return MovingToNeighbour(ResolveAlias(from_alias), ResolveAlias(to_alias),
                           socket_id, depth);
}

///////////////////////////////////////////////
AnalysisTraceBuilder &AnalysisTraceBuilder::Backtracking(uint32_t from_id,
                                                         uint32_t depth) {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::Backtracking;
  ev.depth = depth;
  ev.from_id = from_id;
  m_trace.push_back(std::move(ev));
  return *this;
}
/////////////////////////////////////////////////
AnalysisTraceBuilder &
AnalysisTraceBuilder::BacktrackingNamed(std::string from_alias,
                                        uint32_t depth) {
  return Backtracking(ResolveAlias(from_alias), depth);
}

///////////////////////////////////////////////
AnalysisTraceBuilder &AnalysisTraceBuilder::ValidSubgraphIsolated() {

  return *this;
}
/////////////////////////////////////////////////
AnalysisTrace AnalysisTraceBuilder::Build() const { return m_trace; }

///////////////////////////////////////////////
uint32_t AnalysisTraceBuilder::ResolveAlias(const std::string &alias) const {
  if (!m_alias_to_id)
    throw std::runtime_error(
        "AnalysisTraceBuilder alias map is not bound. Call BindAliases(...) "
        "before using named event helpers.");
  auto it = m_alias_to_id->find(alias);
  if (it == m_alias_to_id->end())
    throw std::runtime_error("Unknown part alias '" + alias + "'");
  return it->second;
}

} // namespace steamrot::tests
