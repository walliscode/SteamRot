/////////////////////////////////////////////////
/// @file
/// @brief Helper utilities for isolated reporter testing
///
/// This file provides utilities to test Catch2 reporters in isolation
/// by capturing their output streams and validating formatting.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <catch2/reporters/catch_reporter_cumulative_base.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Captures output from a stream during a scope
/////////////////////////////////////////////////
class StreamCapture {
private:
  std::ostringstream m_buffer;
  std::streambuf* m_old_buffer;
  std::ostream& m_stream;

public:
  explicit StreamCapture(std::ostream& stream) 
      : m_stream(stream), m_old_buffer(stream.rdbuf()) {
    m_stream.rdbuf(m_buffer.rdbuf());
  }
  
  ~StreamCapture() {
    m_stream.rdbuf(m_old_buffer);
  }
  
  std::string GetCapturedOutput() const {
    return m_buffer.str();
  }
  
  void Clear() {
    m_buffer.str("");
    m_buffer.clear();
  }
};

/////////////////////////////////////////////////
/// @brief Helper to validate reporter output contains expected elements
///
/// @param output The captured output string
/// @param expected_elements Vector of strings that should be present
/// @return true if all elements are found
/////////////////////////////////////////////////
inline bool ValidateReporterOutput(const std::string &output,
                                    const std::vector<std::string> &expected_elements) {
  for (const auto &element : expected_elements) {
    if (output.find(element) == std::string::npos) {
      return false;
    }
  }
  return true;
}

/////////////////////////////////////////////////
/// @brief Count lines in output string
///
/// @param output The string to count lines in
/// @return Number of lines (separated by \n)
/////////////////////////////////////////////////
inline size_t CountLines(const std::string &output) {
  if (output.empty()) return 0;
  
  size_t count = 1;  // Start at 1 since text without \n is still 1 line
  for (char c : output) {
    if (c == '\n') ++count;
  }
  return count;
}

/////////////////////////////////////////////////
/// @brief Check if output has proper formatting structure
///
/// @param output The string to validate
/// @param should_have_dividers Whether dividers (=== or ---) should be present
/// @param should_have_colors Whether color codes might be present (e.g., ANSI)
/// @return true if structure is valid
/////////////////////////////////////////////////
inline bool ValidateReporterStructure(const std::string &output,
                                       bool should_have_dividers = true,
                                       bool should_have_colors = false) {
  if (output.empty()) return false;
  
  if (should_have_dividers) {
    bool has_dividers = output.find("===") != std::string::npos || 
                        output.find("---") != std::string::npos;
    if (!has_dividers) return false;
  }
  
  // Validate minimum content
  if (output.length() < 10) return false;
  
  return true;
}

/////////////////////////////////////////////////
/// @brief Extract all lines containing a specific substring
///
/// @param output The string to search
/// @param substring The substring to look for
/// @return Vector of lines containing the substring
/////////////////////////////////////////////////
inline std::vector<std::string> ExtractLinesContaining(const std::string &output,
                                                        const std::string &substring) {
  std::vector<std::string> result;
  std::istringstream stream(output);
  std::string line;
  
  while (std::getline(stream, line)) {
    if (line.find(substring) != std::string::npos) {
      result.push_back(line);
    }
  }
  
  return result;
}

} // namespace steamrot::tests
