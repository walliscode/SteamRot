/////////////////////////////////////////////////
/// @file
/// @brief Abstract base class for descriptor analysis trace formatters.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AnalysisEvent.h"
#include <string>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @class DescriptorFormatter
/// @brief Pure abstract base for objects that render an @c AnalysisTrace
///        into a string representation.
///
/// Concrete formatters implement @c Format() to produce their target
/// representation (terminal text, JSON, HTML, etc.). Extend the hierarchy
/// by subclassing and implementing the single virtual method.
///
/// Example:
/// @code
/// TerminalDescriptorFormatter fmt;
/// std::string output = fmt.Format(result.m_trace);
/// @endcode
/////////////////////////////////////////////////
class DescriptorFormatter {
public:
  /////////////////////////////////////////////////
  /// @brief Default virtual destructor.
  /////////////////////////////////////////////////
  virtual ~DescriptorFormatter() = default;

  /////////////////////////////////////////////////
  /// @brief Render @p trace to a string.
  ///
  /// @param trace The analysis trace to format.
  /// @return String representation of the trace.
  /////////////////////////////////////////////////
  virtual std::string Format(const AnalysisTrace &trace) const = 0;
};

} // namespace steamrot::logic::descriptors
