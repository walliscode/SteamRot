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
    oss << "m_joint_name: actual='" << actual.name << "', expected='"
        << m_expected.name << "'; ";
  }

  if (actual.connection_number != m_expected.connection_number) {
    oss << "m_number_of_connections differs; ";
  }

  if (actual.global_position != m_expected.global_position) {
    oss << "m_global_position differs; ";
  }

  if (actual.connected_fragments != m_expected.connected_fragments) {
    oss << "m_connected_fragments differs; ";
  }

  // Compare transforms (matrix elements)
  const auto *this_matrix = actual.transform.getMatrix();
  const auto *other_matrix = m_expected.transform.getMatrix();
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

  if (actual.render_overlay.getVertexCount() !=
      m_expected.render_overlay.getVertexCount()) {
    oss << "m_render_overlay vertex count differs; ";
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////
std::string JointEqualsMatcher::describe() const {
  if (m_mismatch_description.empty()) {
    std::ostringstream oss;
    oss << "equals Joint(m_joint_name='" << m_expected.name << "')";
    return oss.str();
  }
  return "Joint mismatch: " + m_mismatch_description;
}

} // namespace steamrot::tests
