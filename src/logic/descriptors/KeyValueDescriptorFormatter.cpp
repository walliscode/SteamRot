/////////////////////////////////////////////////
/// @file
/// @brief Implementation of KeyValueDescriptorFormatter.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "KeyValueDescriptorFormatter.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace steamrot::logic::descriptors {

namespace {

constexpr std::size_t kKeyWidth{16};

std::string Indent(uint32_t depth) { return std::string(depth * 2u, ' '); }

std::string Quote(const std::string &value) { return "\"" + value + "\""; }

std::string FormatNodeLabel(uint32_t part_id, const std::string &alias) {
  return alias.empty() ? "node#" + std::to_string(part_id) : alias;
}

std::string FormatNodeValue(uint32_t part_id, const std::string &alias) {
  return Quote(FormatNodeLabel(part_id, alias));
}

std::string FormatEndpointValue(uint32_t part_id, const std::string &alias,
                                uint32_t socket_id) {
  return FormatNodeValue(part_id, alias) + ".socket#" + std::to_string(socket_id);
}

std::string FormatResultValue(bool result) { return result ? "PASS" : "FAIL"; }

std::string FormatScopeKindValue(ScopeKind scope_kind) {
  switch (scope_kind) {
  case ScopeKind::Node:
    return "Node";
  case ScopeKind::Chain:
    return "Chain";
  case ScopeKind::MachinaArchetype:
    return "MachinaArchetype";
  }

  return "Unknown";
}

std::string HeaderLabel(const AnalysisEvent &event) {
  switch (event.kind) {
  case TraceEventKind::EmtpyPartGraph:
  case TraceEventKind::EmtpyChainSteps:
    return "Empty";
  case TraceEventKind::NodeEval:
    return "NodeEval";
  case TraceEventKind::NodeResult:
    return "NodeResult";
  case TraceEventKind::MovingToNeighbour:
    return "Moving";
  case TraceEventKind::Backtracking:
    return "Backtracking";
  case TraceEventKind::ScopeBegin:
  case TraceEventKind::ScopeEnd:
    return "Scope";
  case TraceEventKind::MachinaPartResult:
    return "MachinaPartResult";
  case TraceEventKind::ValidSubgraphIsolated:
  case TraceEventKind::InvalidSubgraphIsolated:
    return "Subgraph";
  }

  return "Event";
}

void AppendField(std::vector<std::string> &lines, uint32_t depth,
                 std::string_view key, const std::string &value) {
  std::ostringstream oss;
  oss << Indent(depth + 1u) << std::left << std::setw(static_cast<int>(kKeyWidth))
      << (std::string(key) + ":") << value;
  lines.push_back(oss.str());
}

std::vector<std::string> FormatEvent(const AnalysisEvent &event) {
  std::vector<std::string> lines;
  lines.push_back(Indent(event.depth) + HeaderLabel(event));

  switch (event.kind) {
  case TraceEventKind::EmtpyPartGraph:
    AppendField(lines, event.depth, "message", Quote("part graph is empty"));
    break;
  case TraceEventKind::EmtpyChainSteps:
    AppendField(lines, event.depth, "message", Quote("chain has no steps"));
    break;
  case TraceEventKind::ScopeBegin:
    AppendField(lines, event.depth, "depth", std::to_string(event.depth));
    AppendField(lines, event.depth, "scope name", Quote(event.scope_name));
    AppendField(lines, event.depth, "scope kind",
                FormatScopeKindValue(event.scope_kind));
    if (event.anchor_id.has_value()) {
      AppendField(lines, event.depth, "anchor",
                  FormatNodeValue(*event.anchor_id, event.part_id_alias));
    }
    break;
  case TraceEventKind::ScopeEnd:
    AppendField(lines, event.depth, "depth", std::to_string(event.depth));
    AppendField(lines, event.depth, "scope name", Quote(event.scope_name));
    AppendField(lines, event.depth, "scope kind",
                FormatScopeKindValue(event.scope_kind));
    AppendField(lines, event.depth, "result", FormatResultValue(event.result));
    break;
  case TraceEventKind::NodeEval:
    AppendField(lines, event.depth, "depth", std::to_string(event.depth));
    AppendField(lines, event.depth, "node name",
                FormatNodeValue(event.part_id, event.part_id_alias));
    AppendField(lines, event.depth, "predicate name",
                Quote(event.predicate_name));
    break;
  case TraceEventKind::NodeResult:
    AppendField(lines, event.depth, "depth", std::to_string(event.depth));
    AppendField(lines, event.depth, "node name",
                FormatNodeValue(event.part_id, event.part_id_alias));
    AppendField(lines, event.depth, "predicate name",
                Quote(event.predicate_name));
    AppendField(lines, event.depth, "result", FormatResultValue(event.result));
    if (!event.reason.empty()) {
      AppendField(lines, event.depth, "reason", Quote(event.reason));
    }
    break;
  case TraceEventKind::MovingToNeighbour:
    AppendField(lines, event.depth, "depth", std::to_string(event.depth));
    AppendField(lines, event.depth, "from",
                FormatEndpointValue(event.from_id, event.from_id_alias,
                                    event.from_socket_id));
    AppendField(lines, event.depth, "to",
                FormatEndpointValue(event.to_id, event.to_id_alias,
                                    event.to_socket_id));
    break;
  case TraceEventKind::Backtracking:
    AppendField(lines, event.depth, "depth", std::to_string(event.depth));
    AppendField(lines, event.depth, "from",
                FormatEndpointValue(event.from_id, event.from_id_alias,
                                    event.from_socket_id));
    AppendField(lines, event.depth, "to",
                FormatEndpointValue(event.to_id, event.to_id_alias,
                                    event.to_socket_id));
    break;
  case TraceEventKind::MachinaPartResult:
    AppendField(lines, event.depth, "depth", std::to_string(event.depth));
    AppendField(lines, event.depth, "predicate name",
                Quote(event.predicate_name));
    AppendField(lines, event.depth, "result", FormatResultValue(event.result));
    break;
  case TraceEventKind::ValidSubgraphIsolated:
    AppendField(lines, event.depth, "depth", std::to_string(event.depth));
    AppendField(lines, event.depth, "result", FormatResultValue(true));
    AppendField(lines, event.depth, "reason",
                Quote("valid subgraph is isolated"));
    break;
  case TraceEventKind::InvalidSubgraphIsolated:
    AppendField(lines, event.depth, "depth", std::to_string(event.depth));
    AppendField(lines, event.depth, "result", FormatResultValue(false));
    AppendField(lines, event.depth, "reason",
                Quote("invalid subgraph is isolated"));
    break;
  }

  return lines;
}

} // namespace

/////////////////////////////////////////////////
std::string
KeyValueDescriptorFormatter::Format(const AnalysisTrace &trace) const {
  std::ostringstream oss;

  for (std::size_t i = 0; i < trace.size(); ++i) {
    const auto lines = FormatEvent(trace[i]);
    for (const auto &line : lines) {
      oss << line << '\n';
    }

    if (i + 1u < trace.size()) {
      oss << '\n';
    }
  }

  return oss.str();
}

} // namespace steamrot::logic::descriptors
