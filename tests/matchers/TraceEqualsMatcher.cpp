/////////////////////////////////////////////////
/// @file
/// @brief Implementation of TraceEqualsMatcher.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TraceEqualsMatcher.h"
#include <algorithm>
#include <sstream>
#include <vector>

namespace steamrot::tests {

namespace {

/////////////////////////////////////////////////
/// @brief Split @p s into a vector of lines, stripping trailing newlines.
/////////////////////////////////////////////////
std::vector<std::string> SplitLines(const std::string &s) {
  std::vector<std::string> lines;
  std::istringstream stream(s);
  std::string line;
  while (std::getline(stream, line)) {
    lines.push_back(line);
  }
  return lines;
}

} // namespace

/////////////////////////////////////////////////
TraceEqualsMatcher::TraceEqualsMatcher(
    const steamrot::logic::descriptors::AnalysisTrace &expected,
    const steamrot::logic::descriptors::DescriptorFormatter &formatter)
    : m_expected(expected), m_formatter(formatter) {}

/////////////////////////////////////////////////
bool TraceEqualsMatcher::match(
    const steamrot::logic::descriptors::AnalysisTrace &actual) const {
  m_mismatch_description.clear();

  const std::string expected_str = m_formatter.Format(m_expected);
  const std::string actual_str = m_formatter.Format(actual);

  if (actual_str == expected_str)
    return true;

  const auto expected_lines = SplitLines(expected_str);
  const auto actual_lines = SplitLines(actual_str);
  const size_t max_lines =
      std::max(expected_lines.size(), actual_lines.size());

  std::ostringstream oss;
  oss << "expected formatted output:\n";
  if (expected_str.empty()) {
    oss << "  <empty>\n";
  } else {
    for (const auto &ln : expected_lines) {
      oss << "  " << ln << "\n";
    }
  }

  oss << "actual formatted output:\n";
  if (actual_str.empty()) {
    oss << "  <empty>\n";
  } else {
    for (const auto &ln : actual_lines) {
      oss << "  " << ln << "\n";
    }
  }

  for (size_t i = 0; i < max_lines; ++i) {
    const std::string &exp =
        (i < expected_lines.size()) ? expected_lines[i] : "<missing>";
    const std::string &act =
        (i < actual_lines.size()) ? actual_lines[i] : "<missing>";
    if (exp != act) {
      oss << "first mismatch at line " << (i + 1) << ":\n";
      oss << "  expected: \"" << exp << "\"\n";
      oss << "  actual:   \"" << act << "\"\n";
      break;
    }
  }

  m_mismatch_description = oss.str();
  return false;
}

/////////////////////////////////////////////////
std::string TraceEqualsMatcher::describe() const {
  if (m_mismatch_description.empty())
    return "equals trace";
  return "equals trace\n" + m_mismatch_description;
}

} // namespace steamrot::tests
