/////////////////////////////////////////////////
/// @file
/// @brief Implementation of AnalysisTraceBuilder.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AnalysisTraceBuilder.h"

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
AnalysisTraceBuilder &AnalysisTraceBuilder::EmptyChainSteps() {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::EmtpyChainSteps;
  m_trace.push_back(std::move(ev));
  return *this;
}
/////////////////////////////////////////////////
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
AnalysisTraceBuilder::ValidSubgraphIsolated(uint32_t depth) {
  AnalysisEvent ev{};
  ev.kind = TraceEventKind::ValidSubgraphIsolated;
  ev.depth = depth;
  m_trace.push_back(std::move(ev));
  return *this;
}
/////////////////////////////////////////////////
AnalysisTrace AnalysisTraceBuilder::Build() const { return m_trace; }

} // namespace steamrot::tests
