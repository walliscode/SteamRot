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
#include <optional>
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
  
  ////////////////////////////////////////////////////////////
  /// @brief Format context as a string for matcher output
  ///
  /// @return Formatted context string
  ////////////////////////////////////////////////////////////
  std::string ToString() const {
    std::string result;
    
    if (!test_name.empty()) {
      result += "Test: " + test_name;
    }
    
    if (!description.empty()) {
      if (!result.empty()) result += ", ";
      result += "Description: " + description;
    }
    
    if (current_tick.has_value()) {
      if (!result.empty()) result += ", ";
      result += "[Tick " + std::to_string(current_tick.value());
      if (total_ticks.has_value()) {
        result += " of " + std::to_string(total_ticks.value());
      }
      result += "]";
    }
    
    return result;
  }
};

} // namespace steamrot::tests
