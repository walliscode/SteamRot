/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for Joint comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "JointEqualsMatcher.h"
#include <sstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
JointEqualsMatcher::JointEqualsMatcher(const Joint &expected)
    : m_expected(expected) {}

/////////////////////////////////////////////////
bool JointEqualsMatcher::match(const Joint &actual) const {
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
std::string JointEqualsMatcher::describe() const {
  if (m_mismatch_description.empty()) {
    std::ostringstream oss;
    oss << "equals Joint(name='" << m_expected.name << "')";
    return oss.str();
  }
  return "Joint mismatch: " + m_mismatch_description;
}

} // namespace steamrot::tests
