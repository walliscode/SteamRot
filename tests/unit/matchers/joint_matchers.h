/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for Joint comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "Joint.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class Joint specific matcher
/////////////////////////////////////////////////
class JointEqualsMatcher : public Catch::Matchers::MatcherBase<Joint> {
private:
  const Joint &m_expected;
  mutable std::string m_mismatch_description;

public:
  explicit JointEqualsMatcher(const Joint &expected) : m_expected(expected) {}

  bool match(const Joint &actual) const override {
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

  std::string describe() const override {
    std::ostringstream oss;
    oss << "equals Joint(m_joint_name='" << m_expected.m_joint_name << "')";
    return oss.str();
  }

  std::string get_mismatch_description() const { return m_mismatch_description; }
};

inline JointEqualsMatcher EqualsJoint(const Joint &expected) {
  return JointEqualsMatcher(expected);
}

} // namespace steamrot::tests
