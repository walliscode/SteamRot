/////////////////////////////////////////////////
/// @file
/// @brief Test context information for enriched matcher output
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "console_output.h"
#include <optional>
#include <sstream>
#include <string>
namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Context information for test execution
///
/// Contains metadata about the current test that can be included
/// in matcher failure messages for better debugging
/////////////////////////////////////////////////
struct TestContext {
  std::string test_name;
  std::string description;
  std::optional<uint32_t> current_tick;
  std::optional<uint32_t> total_ticks;

  std::string FormatTestName() const {

    if (console::IsColorEnabled()) {
      return "\n  Test: " + std::string(console::Color::Bold) + test_name +
             std::string(console::Color::Reset);
    }
    return "\n  Test: " + test_name;
  }
  ////////////////////////////////////////////////////////////
  /// @brief Format hierarchical context section for matcher output
  ///
  /// Generates formatted, indented lines with test name, tick,
  /// and description. Used by matchers to display context information.
  ///
  /// @return Formatted context section (multi-line with indentation)
  ////////////////////////////////////////////////////////////
  std::string FormatContextSection() const {
    std::ostringstream oss;

    // Test name (primary context)
    if (!test_name.empty()) {
      oss << "\n  Test: " << test_name;
    }

    // Tick information (secondary context)
    if (current_tick.has_value()) {
      oss << "\n  Tick: [" << current_tick.value();
      if (total_ticks.has_value()) {
        oss << " of " << total_ticks.value();
      }
      oss << "]";
    }

    // Description (tertiary context)
    if (!description.empty()) {
      oss << "\n  Description: " << description;
    }

    return oss.str();
  }

  ////////////////////////////////////////////////////////////
  /// @brief Check if context has any information to display
  ///
  /// @return true if any context field is populated
  ////////////////////////////////////////////////////////////
  bool HasContent() const {
    return !test_name.empty() || !description.empty() ||
           current_tick.has_value();
  }

  ////////////////////////////////////////////////////////////
  /// @brief Format complete failure message with header and context
  ///
  /// Generates a full formatted failure message with header,
  /// borders, context section, and differences section placeholder.
  ///
  /// @param comparison_type Type of comparison (e.g., "EntityMemoryPool")
  /// @param differences Detailed difference description
  /// @return Complete formatted failure message
  ////////////////////////////////////////////////////////////
  std::string FormatFailureMessage(const std::string &comparison_type,
                                   const std::string &differences) const {
    std::ostringstream oss;

    // Header with error indicator
    oss << "\n[FAILED] " << comparison_type << " Comparison";
    oss << "\n" << std::string(60, '=');

    // Context section (if available)
    if (HasContent()) {
      oss << FormatContextSection();
    }

    // Separator before differences
    oss << "\n" << std::string(60, '-');

    // Differences section
    if (!differences.empty()) {
      oss << "\n  Differences:";
      oss << "\n    * " << differences;
    }

    // Bottom border
    oss << "\n" << std::string(60, '=');

    return oss.str();
  }
};

} // namespace steamrot::tests
