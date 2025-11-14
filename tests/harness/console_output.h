/////////////////////////////////////////////////
/// @file
/// @brief Utilities for formatted console output in test harness
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <iostream>
#include <optional>
#include <string>

namespace steamrot::tests::console {

/////////////////////////////////////////////////
/// @brief Print a formatted success message with tick box
///
/// @param message Message to display
/// @param tick Optional tick number to include
/////////////////////////////////////////////////
inline void PrintSuccess(const std::string &message,
                         std::optional<uint32_t> tick = std::nullopt) {
  std::cout << "\n✓ ";
  if (tick.has_value()) {
    std::cout << "[Tick " << tick.value() << "] ";
  }
  std::cout << message << std::endl;
}

/////////////////////////////////////////////////
/// @brief Print a formatted error message with cross mark
///
/// @param message Message to display
/// @param tick Optional tick number to include
/////////////////////////////////////////////////
inline void PrintError(const std::string &message,
                       std::optional<uint32_t> tick = std::nullopt) {
  std::cerr << "\n✗ ";
  if (tick.has_value()) {
    std::cerr << "[Tick " << tick.value() << "] ";
  }
  std::cerr << message << std::endl;
}

/////////////////////////////////////////////////
/// @brief Print a formatted info message with bullet point
///
/// @param message Message to display
/// @param tick Optional tick number to include
/////////////////////////////////////////////////
inline void PrintInfo(const std::string &message,
                      std::optional<uint32_t> tick = std::nullopt) {
  std::cout << "\n• ";
  if (tick.has_value()) {
    std::cout << "[Tick " << tick.value() << "] ";
  }
  std::cout << message << std::endl;
}

/////////////////////////////////////////////////
/// @brief Print a formatted section header
///
/// @param title Section title
/////////////////////////////////////////////////
inline void PrintSectionHeader(const std::string &title) {
  std::cout << "\n━━━━ " << title << " ━━━━" << std::endl;
}

/////////////////////////////////////////////////
/// @brief Print a formatted test start message
///
/// @param test_name Name of the test
/////////////////////////////////////////////////
inline void PrintTestStart(const std::string &test_name) {
  std::cout << "\n┌─────────────────────────────────────" << std::endl;
  std::cout << "│ Running Test: " << test_name << std::endl;
  std::cout << "└─────────────────────────────────────" << std::endl;
}

/////////////////////////////////////////////////
/// @brief Print a formatted tick progress message
///
/// @param current_tick Current tick number
/// @param total_ticks Total number of ticks
/////////////////////////////////////////////////
inline void PrintTickProgress(uint32_t current_tick, uint32_t total_ticks) {
  std::cout << "\n➤ Executing Tick " << current_tick << " of " << total_ticks
            << std::endl;
}

/////////////////////////////////////////////////
/// @brief Print a formatted comparison result
///
/// @param success Whether the comparison succeeded
/// @param context Context of the comparison (e.g., "Entity Pool", "Event Bus")
/// @param tick Optional tick number
/////////////////////////////////////////////////
inline void PrintComparisonResult(bool success, const std::string &context,
                                   std::optional<uint32_t> tick = std::nullopt) {
  if (success) {
    PrintSuccess(context + " comparison passed", tick);
  } else {
    PrintError(context + " comparison failed", tick);
  }
}

} // namespace steamrot::tests::console
