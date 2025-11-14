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
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>

namespace steamrot::tests::console {

/////////////////////////////////////////////////
/// @brief ANSI color codes for terminal output
/////////////////////////////////////////////////
namespace Color {
constexpr const char *Reset = "\033[0m";
constexpr const char *Bold = "\033[1m";
constexpr const char *Green = "\033[32m";
constexpr const char *Red = "\033[31m";
constexpr const char *Blue = "\033[34m";
constexpr const char *Cyan = "\033[36m";
constexpr const char *Yellow = "\033[33m";
constexpr const char *Magenta = "\033[35m";
constexpr const char *BoldGreen = "\033[1;32m";
constexpr const char *BoldRed = "\033[1;31m";
constexpr const char *BoldBlue = "\033[1;34m";
constexpr const char *BoldCyan = "\033[1;36m";
constexpr const char *BoldYellow = "\033[1;33m";
} // namespace Color

/////////////////////////////////////////////////
/// @brief Check if color output is enabled
///
/// Color is enabled by default unless STEAMROT_NO_COLOR environment
/// variable is set to any value, or NO_COLOR is set (standard convention).
///
/// @return true if colors should be used, false otherwise
/////////////////////////////////////////////////
inline bool IsColorEnabled() {
  static bool checked = false;
  static bool enabled = true;

  if (!checked) {
    // Check for NO_COLOR (standard convention) or STEAMROT_NO_COLOR
    const char *no_color = std::getenv("NO_COLOR");
    const char *steamrot_no_color = std::getenv("STEAMROT_NO_COLOR");
    enabled = (no_color == nullptr && steamrot_no_color == nullptr);
    checked = true;
  }

  return enabled;
}

/////////////////////////////////////////////////
/// @brief Print a formatted success message with tick box
///
/// @param message Message to display
/// @param tick Optional tick number to include
/////////////////////////////////////////////////
inline void PrintSuccess(const std::string &message,
                         std::optional<uint32_t> tick = std::nullopt) {
  if (IsColorEnabled()) {
    std::cout << "\n" << Color::BoldGreen << "✓ " << Color::Reset;
  } else {
    std::cout << "\n✓ ";
  }
  if (tick.has_value()) {
    if (IsColorEnabled()) {
      std::cout << Color::Cyan << "[Tick " << tick.value() << "] "
                << Color::Reset;
    } else {
      std::cout << "[Tick " << tick.value() << "] ";
    }
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
  if (IsColorEnabled()) {
    std::cerr << "\n" << Color::BoldRed << "✗ " << Color::Reset;
  } else {
    std::cerr << "\n✗ ";
  }
  if (tick.has_value()) {
    if (IsColorEnabled()) {
      std::cerr << Color::Cyan << "[Tick " << tick.value() << "] "
                << Color::Reset;
    } else {
      std::cerr << "[Tick " << tick.value() << "] ";
    }
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
  if (IsColorEnabled()) {
    std::cout << "\n" << Color::Blue << "• " << Color::Reset;
  } else {
    std::cout << "\n• ";
  }
  if (tick.has_value()) {
    if (IsColorEnabled()) {
      std::cout << Color::Cyan << "[Tick " << tick.value() << "] "
                << Color::Reset;
    } else {
      std::cout << "[Tick " << tick.value() << "] ";
    }
  }
  std::cout << message << std::endl;
}

/////////////////////////////////////////////////
/// @brief Print a formatted section header
///
/// @param title Section title
/////////////////////////////////////////////////
inline void PrintSectionHeader(const std::string &title) {
  if (IsColorEnabled()) {
    std::cout << "\n" << Color::BoldYellow << "━━━━ " << title << " ━━━━"
              << Color::Reset << std::endl;
  } else {
    std::cout << "\n━━━━ " << title << " ━━━━" << std::endl;
  }
}

/////////////////////////////////////////////////
/// @brief Print a formatted test start message
///
/// @param test_name Name of the test
/////////////////////////////////////////////////
inline void PrintTestStart(const std::string &test_name) {
  if (IsColorEnabled()) {
    std::cout << "\n" << Color::BoldCyan
              << "┌─────────────────────────────────────" << std::endl;
    std::cout << "│ Running Test: " << Color::Reset << Color::Bold
              << test_name << Color::Reset << std::endl;
    std::cout << Color::BoldCyan << "└─────────────────────────────────────"
              << Color::Reset << std::endl;
  } else {
    std::cout << "\n┌─────────────────────────────────────" << std::endl;
    std::cout << "│ Running Test: " << test_name << std::endl;
    std::cout << "└─────────────────────────────────────" << std::endl;
  }
}

/////////////////////////////////////////////////
/// @brief Print a formatted tick progress message
///
/// @param current_tick Current tick number
/// @param total_ticks Total number of ticks
/////////////////////////////////////////////////
inline void PrintTickProgress(uint32_t current_tick, uint32_t total_ticks) {
  if (IsColorEnabled()) {
    std::cout << "\n" << Color::Magenta << "➤ " << Color::Reset
              << "Executing Tick " << Color::BoldBlue << current_tick
              << Color::Reset << " of " << Color::BoldBlue << total_ticks
              << Color::Reset << std::endl;
  } else {
    std::cout << "\n➤ Executing Tick " << current_tick << " of " << total_ticks
              << std::endl;
  }
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
