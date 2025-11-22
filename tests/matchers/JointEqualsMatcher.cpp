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

  if (actual.m_joint_name != m_expected.m_joint_name) {
    oss << "m_joint_name: actual='" << actual.m_joint_name << "', expected='"
        << m_expected.m_joint_name << "'; ";
  }

  if (actual.m_number_of_connections != m_expected.m_number_of_connections) {
    oss << "m_number_of_connections differs; ";
  }

  if (actual.m_global_position != m_expected.m_global_position) {
    oss << "m_global_position differs; ";
  }

  if (actual.m_connected_fragments != m_expected.m_connected_fragments) {
    oss << "m_connected_fragments differs; ";
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

  if (actual.m_render_overlay.getVertexCount() !=
      m_expected.m_render_overlay.getVertexCount()) {
    oss << "m_render_overlay vertex count differs; ";
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////
std::string JointEqualsMatcher::describe() const {
  if (m_mismatch_description.empty()) {
    std::ostringstream oss;
    oss << "equals Joint(m_joint_name='" << m_expected.m_joint_name << "')";
    return oss.str();
  }
  return "Joint mismatch: " + m_mismatch_description;
}

} // namespace steamrot::tests
