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
  std::string given;        // GIVEN: Initial state/preconditions
  std::string when;         // WHEN: Action/event being tested
  std::string then;         // THEN: Expected outcome/postconditions
  std::optional<uint32_t> current_tick;
  std::optional<uint32_t> total_ticks;

  std::string FormatTestName() const {

    if (console::IsColorEnabled()) {
      return "test: " + std::string(console::Color::Bold) + test_name +
             std::string(console::Color::Reset);
    }
    return "test: " + test_name;
  }

  std::string FormatTickInfo() const {
    if (current_tick.has_value()) {
      std::ostringstream oss;
      oss << "tick: [" << current_tick.value();
      if (total_ticks.has_value()) {
        oss << "/" << total_ticks.value();
      }
      oss << "]";
      return oss.str();
    }
    return "tick: [N/A]";
  }
  ////////////////////////////////////////////////////////////
  /// @brief Format hierarchical context section for matcher output
  ///
  /// Generates formatted, indented lines with test name, tick,
  /// and GIVEN/WHEN/THEN. Used by matchers to display context information.
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

    // GIVEN/WHEN/THEN structure
    if (!given.empty()) {
      oss << "\n  GIVEN: " << given;
    }
    if (!when.empty()) {
      oss << "\n  WHEN:  " << when;
    }
    if (!then.empty()) {
      oss << "\n  THEN:  " << then;
    }

    return oss.str();
  }

  ////////////////////////////////////////////////////////////
  /// @brief Check if context has any information to display
  ///
  /// @return true if any context field is populated
  ////////////////////////////////////////////////////////////
  bool HasContent() const {
    return !test_name.empty() ||
           !given.empty() || !when.empty() || !then.empty() ||
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
