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

  void AddScopeBeginHeading(std::string &indented_string,
                            const ScopeKind &scope_kind,
                            const std::string &scope_name) const;

  void AddScopeEndHeading(std::string &indented_string,
                          const ScopeKind &scope_kind,
                          const std::string &scope_name, bool result) const;

  void AddNodeEval(std::string &indented_string, const AnalysisEvent &ev,
                   const uint32_t depth) const;

  /////////////////////////////////////////////////
  /// @brief Render @p trace as indented plain text.
  ///
  /// @param trace The analysis trace to format.
  /// @return Newline-terminated string with one line per event.
  /////////////////////////////////////////////////
  std::string Format(const AnalysisTrace &trace) const override;

  std::string FormatEvent(const AnalysisEvent &ev) const;
};

} // namespace steamrot::logic::descriptors
