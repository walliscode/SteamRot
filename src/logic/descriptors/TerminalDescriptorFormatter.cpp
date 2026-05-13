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
/// @brief Format a single AnalysisEvent as a terminal line.
/////////////////////////////////////////////////
std::string FormatEvent(const AnalysisEvent &ev) {
  const std::string indent = Indent(ev.depth);

  switch (ev.kind) {
  case TraceEventKind::ScopeBegin: {
    std::string line = indent + "[";
    switch (ev.scope_kind) {
    case ScopeKind::Chain:
      line += "CHAIN";
      break;
    case ScopeKind::Graph:
      line += "GRAPH";
      break;
    case ScopeKind::Node:
      line += "SCOPE";
      break;
    }
    line += "] " + ev.scope_name;
    if (ev.anchor_id.has_value()) {
      line += "  anchor=node#" + std::to_string(*ev.anchor_id);
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
    return indent + "[EVAL]  node#" + std::to_string(ev.part_id) +
           "  predicate=" + ev.predicate_name;
  }

  case TraceEventKind::NodeResult: {
    std::string line = indent;
    line += ev.result ? "[PASS]" : "[FAIL]";
    line += "  node#" + std::to_string(ev.part_id) + "  " + ev.predicate_name;
    if (!ev.reason.empty()) {
      line += "  \"" + ev.reason + "\"";
    }
    return line;
  }

  case TraceEventKind::MovingToNeighbour: {
    return indent + "[MOVE]  node#" + std::to_string(ev.from_id) +
           " -> node#" + std::to_string(ev.to_id) +
           "  socket=" + std::to_string(ev.socket_id);
  }

  case TraceEventKind::Backtracking: {
    return indent + "[BACK]  <- node#" + std::to_string(ev.from_id);
  }
  }

  return indent + "[?]";
}

} // namespace

/////////////////////////////////////////////////
std::string TerminalDescriptorFormatter::Format(const AnalysisTrace &trace) const {
  std::string output;
  output.reserve(trace.size() * 40);
  for (const AnalysisEvent &ev : trace) {
    output += FormatEvent(ev);
    output += '\n';
  }
  return output;
}

} // namespace steamrot::logic::descriptors
