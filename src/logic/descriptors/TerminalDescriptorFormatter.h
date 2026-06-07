/////////////////////////////////////////////////
/// @file
/// @brief Declaration of TerminalDescriptorFormatter.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AnalysisEvent.h"
#include "DescriptorFormatter.h"
#include <cstdint>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @class TerminalDescriptorFormatter
/// @brief Formats an @c AnalysisTrace as a human-readable, indented plain-text
///        string suitable for terminal or log output.

/////////////////////////////////////////////////
class TerminalDescriptorFormatter : public DescriptorFormatter {
public:
  std::string Indent(uint32_t depth) const;

  /////////////////////////////////////////////////
  /// @brief Add a heading for the beginning of a descriptor scope (e.g. chain
  /// or archetype) to @p indented_string.
  ///
  /// @param indented_string string to which the heading will be appended.
  /// Should already be indented to the appropriate depth.
  /// @param scope_kind  kind of the scope (e.g. chain or archetype) that is
  /// beginning.
  /// @param scope_name name of the scope (e.g. the chain's name).
  /////////////////////////////////////////////////
  void AddScopeBeginHeading(std::string &indented_string,
                            const ScopeKind &scope_kind,
                            const std::string &scope_name) const;

  /////////////////////////////////////////////////
  /// @brief Add a heading for the end of a descriptor scope (e.g. chain or
  /// archetype) to @p indented_string, including the result of the scope's
  /// evaluation.
  ///
  /// @param indented_string string to which the heading will be appended.
  /// Should already be indented to the appropriate depth.
  /// @param scope_kind  kind of the scope (e.g. chain or archetype) that is
  /// ending.
  /// @param scope_name  name of the scope (e.g. the chain's name).
  /// @param result outcome of the scope evaluation, where true indicates the
  /// scope passed and false indicates it failed.
  /////////////////////////////////////////////////
  void AddScopeEndHeading(std::string &indented_string,
                          const ScopeKind &scope_kind,
                          const std::string &scope_name, bool result) const;

  /////////////////////////////////////////////////
  /// @brief Add a line describing a NodeEval event to @p indented_string,
  /// including
  ///
  /// @param indented_string string to which the line will be appended. Should
  /// already be indented to the appropriate depth.
  /// @param ev An AnalysisEvent of kind NodeEval whose details will be rendered
  /// in the line.
  /// @param depth Nesting depth of the event, used to determine the indentation
  /// level of the line.
  /////////////////////////////////////////////////
  void AddNodeEval(std::string &indented_string, const AnalysisEvent &ev,
                   const uint32_t depth) const;

  /////////////////////////////////////////////////
  /// @brief Add a line describing a MovingToNeighbour event to @p
  /// indented_string, including
  ///
  /// @param indented_string string to which the line will be appended. Should
  /// already be indented to the appropriate depth.
  /// @param ev  An AnalysisEvent of kind MovingToNeighbour whose details will
  /// be rendered in the line.
  /////////////////////////////////////////////////
  void AddMovingToNeighbour(std::string &indented_string,
                            const AnalysisEvent &ev) const;

  /////////////////////////////////////////////////
  /// @brief Add a line describing a Backtracking event to @p indented_string,
  /// including
  ///
  /// @param indented_string string to which the line will be appended. Should
  /// already be indented to the appropriate depth.
  /// @param ev An AnalysisEvent of kind Backtracking whose details will be
  /// rendered in the line.
  /////////////////////////////////////////////////
  void AddBacktracking(std::string &indented_string,
                       const AnalysisEvent &ev) const;

  /////////////////////////////////////////////////
  /// @brief Render @p trace as indented plain text.
  ///
  /// @param trace The analysis trace to format.
  /// @return Newline-terminated string with one line per event.
  /////////////////////////////////////////////////
  std::string Format(const AnalysisTrace &trace) const override;

  std::string FormatEvent(const AnalysisEvent &ev) const;

  std::string FormatEndpoint(const std::string &node_label,
                             uint32_t socket_id) const;
};

} // namespace steamrot::logic::descriptors
