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

  if (actual.name != m_expected.name) {
    oss << "name: actual='" << actual.name << "', expected='"
        << m_expected.name << "'; ";
  }

  if (actual.sockets != m_expected.sockets) {
    oss << "sockets differs; ";
  }

  if (actual.movement_views != m_expected.movement_views) {
    oss << "movement_views differs; ";
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////
std::string FragmentEqualsMatcher::describe() const {
  if (m_mismatch_description.empty()) {
    std::ostringstream oss;
    oss << "equals Fragment(name='" << m_expected.name << "')";
    return oss.str();
  }
  return "Fragment mismatch: " + m_mismatch_description;
}

} // namespace steamrot::tests
