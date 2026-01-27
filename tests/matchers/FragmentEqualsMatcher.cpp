/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for Fragment comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FragmentEqualsMatcher.h"
#include <sstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
FragmentEqualsMatcher::FragmentEqualsMatcher(const Fragment &expected)
    : m_expected(expected) {}

/////////////////////////////////////////////////
bool FragmentEqualsMatcher::match(const Fragment &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  if (actual.m_name != m_expected.m_name) {
    oss << "m_name: actual='" << actual.m_name << "', expected='"
        << m_expected.m_name << "'; ";
  }

  if (actual.m_sockets != m_expected.m_sockets) {
    oss << "m_sockets differs; ";
  }

  // Compare transforms (matrix elements)
  const auto *this_matrix = actual.m_transform.getMatrix();
  const auto *other_matrix = m_expected.m_transform.getMatrix();
  bool transform_matches = true;
  for (size_t i = 0; i < 16; ++i) {
    if (this_matrix[i] != other_matrix[i]) {
      transform_matches = false;
      break;
    }
  }
  if (!transform_matches) {
    oss << "m_transform differs; ";
  }

  if (actual.m_overlays.size() != m_expected.m_overlays.size()) {
    oss << "m_overlays size differs; ";
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////
std::string FragmentEqualsMatcher::describe() const {
  if (m_mismatch_description.empty()) {
    std::ostringstream oss;
    oss << "equals Fragment(m_name='" << m_expected.m_name << "')";
    return oss.str();
  }
  return "Fragment mismatch: " + m_mismatch_description;
}

} // namespace steamrot::tests
