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
#include "DescriptorFormatter.h"

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @class TerminalDescriptorFormatter
/// @brief Formats an @c AnalysisTrace as a human-readable, indented plain-text
///        string suitable for terminal or log output.
///
/// Each event is rendered on its own line. Indentation is 2 spaces per depth
/// level. The output format per event kind is:
///
/// @code
/// [CHAIN] chain_name  anchor=node#<id>
///   [EVAL]  node#<id>  predicate=<name>
///   [PASS]  node#<id>  <name>  "<reason>"
///   [FAIL]  node#<id>  <name>  "<reason>"
///   [MOVE]  node#<from>.socket#<from_socket> ->
///           node#<to>.socket#<to_socket>
///   [BACK]  node#<from>.socket#<from_socket> ->
///           node#<to>.socket#<to_socket>
/// [PASS] chain_name
/// [FAIL] chain_name
/// @endcode
/////////////////////////////////////////////////
class TerminalDescriptorFormatter : public DescriptorFormatter {
public:
  /////////////////////////////////////////////////
  /// @brief Render @p trace as indented plain text.
  ///
  /// @param trace The analysis trace to format.
  /// @return Newline-terminated string with one line per event.
  /////////////////////////////////////////////////
  std::string Format(const AnalysisTrace &trace) const override;
};

} // namespace steamrot::logic::descriptors
