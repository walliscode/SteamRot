/////////////////////////////////////////////////
/// @file
/// @brief Implementation of TerminalDescriptorFormatter.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TerminalDescriptorFormatter.h"
#include <string>

namespace steamrot::logic::descriptors {

namespace {

/////////////////////////////////////////////////
/// @brief Return a string of @p depth * 2 spaces.
/////////////////////////////////////////////////
std::string Indent(uint32_t depth) { return std::string(depth * 2u, ' '); }

/////////////////////////////////////////////////
/// @brief Format a node/socket endpoint label.
/////////////////////////////////////////////////
std::string FormatEndpoint(const std::string &node_label, uint32_t socket_id) {
  return node_label + ".socket#" + std::to_string(socket_id);
}

/////////////////////////////////////////////////
/// @brief Format a single AnalysisEvent as a terminal line.
/////////////////////////////////////////////////
std::string FormatEvent(const AnalysisEvent &ev) {
  const std::string indent = Indent(ev.depth);

  switch (ev.kind) {
  case TraceEventKind::EmtpyPartGraph: {
    return indent + "[EMPTY] part graph is empty";
  }
  case TraceEventKind::EmtpyChainSteps: {
    return indent + "[EMPTY] chain has no steps";
  }
  case TraceEventKind::ScopeBegin: {
    std::string line = indent + "[";
    switch (ev.scope_kind) {
    case ScopeKind::Chain:
      line += "CHAIN";
      break;
    case ScopeKind::MachinaArchetype:
      line += "MACHINA_ARCHETYPE";
      break;
    case ScopeKind::Node:
      line += "SCOPE";
      break;
    }
    line += "] " + ev.scope_name;
    if (ev.anchor_id.has_value()) {
      const std::string anchor_label =
          ev.part_id_alias.empty() ? "node#" + std::to_string(*ev.anchor_id)
                                   : ev.part_id_alias;
      line += "  anchor=" + anchor_label;
    }
    return line;
  }

  case TraceEventKind::ScopeEnd: {
    std::string line = indent;
    line += ev.result ? "[PASS]" : "[FAIL]";
    line += " " + ev.scope_name;
    return line;
  }

  case TraceEventKind::NodeEval: {
    const std::string node_label = ev.part_id_alias.empty()
                                       ? "node#" + std::to_string(ev.part_id)
                                       : ev.part_id_alias;
    return indent + "[EVAL]  " + node_label +
           "  predicate=" + ev.predicate_name;
  }

  case TraceEventKind::NodeResult: {
    const std::string node_label = ev.part_id_alias.empty()
                                       ? "node#" + std::to_string(ev.part_id)
                                       : ev.part_id_alias;
    std::string line = indent;
    line += ev.result ? "[PASS]" : "[FAIL]";
    line += "  " + node_label + "  " + ev.predicate_name;
    if (!ev.reason.empty()) {
      line += "  \"" + ev.reason + "\"";
    }
    return line;
  }

  case TraceEventKind::MovingToNeighbour: {
    const std::string from_label = ev.from_id_alias.empty()
                                       ? "node#" + std::to_string(ev.from_id)
                                       : ev.from_id_alias;
    const std::string to_label = ev.to_id_alias.empty()
                                     ? "node#" + std::to_string(ev.to_id)
                                     : ev.to_id_alias;
    return indent + "[MOVE]  " + FormatEndpoint(from_label, ev.from_socket_id) +
           " -> " + FormatEndpoint(to_label, ev.to_socket_id);
  }

  case TraceEventKind::Backtracking: {
    const std::string from_label = ev.from_id_alias.empty()
                                       ? "node#" + std::to_string(ev.from_id)
                                       : ev.from_id_alias;
    const std::string to_label = ev.to_id_alias.empty()
                                     ? "node#" + std::to_string(ev.to_id)
                                     : ev.to_id_alias;
    return indent + "[BACK]  " + FormatEndpoint(from_label, ev.from_socket_id) +
           " -> " + FormatEndpoint(to_label, ev.to_socket_id);
  }

  case TraceEventKind::ValidSubgraphIsolated: {
    return indent + "[PASS]  valid subgraph is isolated";
  }
  case TraceEventKind::InvalidSubgraphIsolated: {
    return indent + "[FAIL]  invalid subgraph is isolated";
  }
  }

  return indent + "[?]";
}

} // namespace

/////////////////////////////////////////////////
std::string
TerminalDescriptorFormatter::Format(const AnalysisTrace &trace) const {
  std::string output;
  output.reserve(trace.size() * 40);
  for (const AnalysisEvent &ev : trace) {
    output += FormatEvent(ev);
    output += '\n';
  }
  return output;
}

} // namespace steamrot::logic::descriptors
