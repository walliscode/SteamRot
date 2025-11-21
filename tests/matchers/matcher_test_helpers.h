/////////////////////////////////////////////////
/// @file
/// @brief Helper utilities for isolated matcher testing
///
/// This file provides utilities to test Catch2 matchers in isolation
/// by extracting their string outputs directly without requiring
/// failing test assertions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <catch2/matchers/catch_matchers.hpp>
#include <string>
#include <sstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Result of a matcher test containing match status and description
/////////////////////////////////////////////////
struct MatcherTestResult {
  bool matched;
  std::string description;
};

/////////////////////////////////////////////////
/// @brief Test a Catch2 matcher in isolation and extract its output
///
/// This function executes a matcher's match() and describe() methods
/// without requiring a Catch2 assertion. This allows testing matcher
/// string generation independently.
///
/// @tparam T The type being matched
/// @tparam Matcher The matcher type (must be derived from Catch::Matchers::MatcherBase<T>)
/// @param actual The actual value to test
/// @param matcher The matcher instance
/// @return MatcherTestResult with match status and description string
/////////////////////////////////////////////////
template <typename T, typename Matcher>
MatcherTestResult TestMatcherOutput(const T &actual, const Matcher &matcher) {
  static_assert(std::is_base_of_v<Catch::Matchers::MatcherBase<T>, Matcher>,
                "Matcher must derive from Catch::Matchers::MatcherBase<T>");
  
  MatcherTestResult result;
  result.matched = matcher.match(actual);
  result.description = matcher.describe();
  
  return result;
}

/////////////////////////////////////////////////
/// @brief Extract just the description string from a matcher without checking match
///
/// Useful for testing the describe() method's output format regardless
/// of whether the match succeeded or failed.
///
/// @tparam T The type being matched
/// @tparam Matcher The matcher type
/// @param matcher The matcher instance
/// @return The description string from describe()
/////////////////////////////////////////////////
template <typename T, typename Matcher>
std::string GetMatcherDescription(const Matcher &matcher) {
  static_assert(std::is_base_of_v<Catch::Matchers::MatcherBase<T>, Matcher>,
                "Matcher must derive from Catch::Matchers::MatcherBase<T>");
  
  return matcher.describe();
}

/////////////////////////////////////////////////
/// @brief Helper to validate that a string contains expected substrings
///
/// @param output The string to search
/// @param expected_substrings Vector of substrings that should be present
/// @return true if all substrings are found
/////////////////////////////////////////////////
inline bool ContainsAllSubstrings(const std::string &output,
                                   const std::vector<std::string> &expected_substrings) {
  for (const auto &substring : expected_substrings) {
    if (output.find(substring) == std::string::npos) {
      return false;
    }
  }
  return true;
}

/////////////////////////////////////////////////
/// @brief Helper to validate that a string does not contain any of the given substrings
///
/// @param output The string to search
/// @param unexpected_substrings Vector of substrings that should not be present
/// @return true if none of the substrings are found
/////////////////////////////////////////////////
inline bool ContainsNoneOfSubstrings(const std::string &output,
                                      const std::vector<std::string> &unexpected_substrings) {
  for (const auto &substring : unexpected_substrings) {
    if (output.find(substring) != std::string::npos) {
      return false;
    }
  }
  return true;
}

/////////////////////////////////////////////////
/// @brief Helper to count occurrences of a substring in a string
///
/// @param text The string to search
/// @param substring The substring to count
/// @return Number of times substring appears in text
/////////////////////////////////////////////////
inline size_t CountSubstring(const std::string &text, const std::string &substring) {
  if (substring.empty()) return 0;
  
  size_t count = 0;
  size_t pos = 0;
  
  while ((pos = text.find(substring, pos)) != std::string::npos) {
    ++count;
    pos += substring.length();
  }
  
  return count;
}

/////////////////////////////////////////////////
/// @brief Helper to validate output format structure
///
/// Checks that the output contains the expected structural elements
/// like dividers, headers, etc.
///
/// @param output The string to check
/// @param should_have_dividers Whether dividers should be present
/// @param should_have_passed Whether PASSED indicator should be present
/// @param should_have_failed Whether FAILED indicator should be present
/// @return true if structure matches expectations
/////////////////////////////////////////////////
inline bool ValidateOutputStructure(const std::string &output,
                                     bool should_have_dividers = true,
                                     bool should_have_passed = false,
                                     bool should_have_failed = false) {
  bool has_dividers = output.find("===") != std::string::npos || 
                       output.find("---") != std::string::npos;
  bool has_passed = output.find("PASSED") != std::string::npos;
  bool has_failed = output.find("FAILED") != std::string::npos;
  
  if (should_have_dividers && !has_dividers) return false;
  if (should_have_passed && !has_passed) return false;
  if (should_have_failed && !has_failed) return false;
  
  return true;
}

} // namespace steamrot::tests
