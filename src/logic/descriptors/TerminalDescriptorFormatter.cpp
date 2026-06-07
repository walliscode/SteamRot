/////////////////////////////////////////////////
/// @file
/// @brief Implementation of TerminalDescriptorFormatter.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TerminalDescriptorFormatter.h"
#include "conmat.h"
#include <cstdint>
#include <string>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @brief Return a string of @p depth * 2 spaces.
/////////////////////////////////////////////////
std::string TerminalDescriptorFormatter::Indent(uint32_t depth) const {
  return std::string(depth * 2u, ' ');
}

/////////////////////////////////////////////////
void TerminalDescriptorFormatter::AddScopeBeginHeading(
    std::string &indented_string, const ScopeKind &scope_kind,
    const std::string &scope_name) const {

  // add the scope kind and name
  indented_string += "[";

  // add the scope kind with formatting
  std::string scope_kind_str;
  switch (scope_kind) {
  case ScopeKind::Chain:
    scope_kind_str += "CHAIN";
    break;
  case ScopeKind::MachinaArchetype:
    scope_kind_str += "MACHINA_ARCHETYPE";
    break;
  case ScopeKind::Node:
    scope_kind_str += "SCOPE";
    break;
  }
  indented_string += conmat::Colorize(scope_kind_str, conmat::Color::Blue);
  indented_string += ": " + scope_name + "]";
}
/////////////////////////////////////////////////
void TerminalDescriptorFormatter::AddScopeEndHeading(
    std::string &indented_string, const ScopeKind &scope_kind,
    const std::string &scope_name, bool result) const {

  indented_string += "[";

  // add the scope kind with formatting
  std::string scope_kind_str;
  switch (scope_kind) {
  case ScopeKind::Chain:
    scope_kind_str += "CHAIN";
    break;
  case ScopeKind::MachinaArchetype:
    scope_kind_str += "MACHINA_ARCHETYPE";
    break;
  case ScopeKind::Node:
    scope_kind_str += "SCOPE";
    break;
  }
  indented_string += conmat::Colorize(scope_kind_str, conmat::Color::Blue);

  // add a space
  indented_string += ": ";

  // add the result with formatting
  result ? indented_string += conmat::Colorize("PASS", conmat::Color::Green)
         : indented_string += conmat::Colorize("FAIL", conmat::Color::Red);

  // finish off the heading
  indented_string += "]";
}

/////////////////////////////////////////////////
void TerminalDescriptorFormatter::AddNodeEval(std::string &indented_string,
                                              const AnalysisEvent &ev,
                                              const uint32_t depth) const {
  const std::string node_label =
      ev.part_id_alias.empty() ? std::to_string(ev.part_id) : ev.part_id_alias;
  indented_string += "[";
  indented_string += conmat::Colorize("NODE EVAL", conmat::Color::Yellow);
  indented_string += "]";
  indented_string += "\n";
  indented_string += Indent(depth + 1);
  indented_string += conmat::Stylize("node: ", conmat::Style::Dim) + node_label;
  indented_string += "\n";
  indented_string += Indent(depth + 1);
  indented_string +=
      conmat::Stylize("predicate: ", conmat::Style::Dim) + ev.predicate_name;
  indented_string += "\n";
  indented_string += Indent(depth + 1);
  indented_string +=

      conmat::Stylize("result: ", conmat::Style::Dim) +
      std::string(ev.result ? conmat::Colorize("PASS", conmat::Color::Green)
                            : conmat::Colorize("FAIL", conmat::Color::Red));
  indented_string += "\n";
  indented_string += Indent(depth + 1);
  indented_string +=
      conmat::Stylize("reason: ", conmat::Style::Dim) + ev.reason;
}

/////////////////////////////////////////////////
void TerminalDescriptorFormatter::AddMovingToNeighbour(
    std::string &indented_string, const AnalysisEvent &ev) const {
  const std::string from_label = ev.from_id_alias.empty()
                                     ? "node#" + std::to_string(ev.from_id)
                                     : ev.from_id_alias;
  const std::string to_label = ev.to_id_alias.empty()
                                   ? "node#" + std::to_string(ev.to_id)
                                   : ev.to_id_alias;
  indented_string += "[";
  indented_string += conmat::Colorize("MOVE", conmat::Color::Cyan);
  indented_string += "]";
  indented_string += Indent(1);
  indented_string += FormatEndpoint(from_label, ev.from_socket_id) + " -> " +
                     FormatEndpoint(to_label, ev.to_socket_id);
}

/////////////////////////////////////////////////
void TerminalDescriptorFormatter::AddBacktracking(
    std::string &indented_string, const AnalysisEvent &ev) const {
  const std::string from_label = ev.from_id_alias.empty()
                                     ? "node#" + std::to_string(ev.from_id)
                                     : ev.from_id_alias;
  const std::string to_label = ev.to_id_alias.empty()
                                   ? "node#" + std::to_string(ev.to_id)
                                   : ev.to_id_alias;
  indented_string += "[";
  indented_string += conmat::Colorize("BACK", conmat::Color::Red);
  indented_string += "]";
  indented_string += Indent(1);
  indented_string += FormatEndpoint(from_label, ev.from_socket_id) + " -> " +
                     FormatEndpoint(to_label, ev.to_socket_id);
}
/////////////////////////////////////////////////
/// @brief Format a node/socket endpoint label.
/////////////////////////////////////////////////
std::string
TerminalDescriptorFormatter::FormatEndpoint(const std::string &node_label,
                                            uint32_t socket_id) const {
  return node_label + ".socket#" + std::to_string(socket_id);
}

/////////////////////////////////////////////////
/// @brief Format a single AnalysisEvent as a terminal line.
/////////////////////////////////////////////////
std::string
TerminalDescriptorFormatter::FormatEvent(const AnalysisEvent &ev) const {
  std::string indent = Indent(ev.depth);

  switch (ev.kind) {
  case TraceEventKind::EmtpyPartGraph: {
    indent += "[";
    indent += conmat::Colorize("EMPTY", conmat::Color::Red);
    indent += "]  ";
    indent += "part graph is empty";
    return indent;
  }
  case TraceEventKind::EmtpyChainSteps: {

    indent += "[";
    indent += conmat::Colorize("EMPTY", conmat::Color::Red);
    indent += "]  ";
    indent += "chain has no steps";
    return indent;
  }
  case TraceEventKind::ScopeBegin: {
    AddScopeBeginHeading(indent, ev.scope_kind, ev.scope_name);
    return indent;
  }

  case TraceEventKind::ScopeEnd: {
    AddScopeEndHeading(indent, ev.scope_kind, ev.scope_name, ev.result);
    return indent;
  }

  case TraceEventKind::NodeEval: {
    AddNodeEval(indent, ev, ev.depth);
    return indent;
  }

  case TraceEventKind::MovingToNeighbour: {
    AddMovingToNeighbour(indent, ev);
    return indent;
  }

  case TraceEventKind::Backtracking: {
    AddBacktracking(indent, ev);
    return indent;
  }

  case TraceEventKind::ValidSubgraphIsolated: {
    return indent + "[PASS]  valid subgraph is isolated";
  }
  case TraceEventKind::InvalidSubgraphIsolated: {
    return indent + "[FAIL]  invalid subgraph is isolated";
  }
  case TraceEventKind::MachinaPartResult: {
    std::string line = Indent(ev.depth + 1);

    if (ev.result) {
      line += "[";
      line += conmat::Colorize("ASSIGNMENT", conmat::Color::Green);
      line += "]";
      line += " part successfully assigned to archtype storage";
    } else {
      line += "[";
      line += conmat::Colorize("ASSIGNMENT", conmat::Color::Red);
      line += "]";
      line += " part failed to be assigned to archetype storage";
    }
    return line;
  }

  default:
    return indent + "[?]";
  }
}

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
