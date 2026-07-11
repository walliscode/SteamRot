/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for comparing sf::Transform objects
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TransformEqualsMatcher.h"
#include "conmat.h"
#include <array>
#include <iomanip>

namespace steamrot::tests {

/////////////////////////////////////////////////
TransformEqualsMatcher::TransformEqualsMatcher(const sf::Transform &expected,
                                               float epsilon)
    : m_expected(expected), m_epsilon(epsilon) {}

/////////////////////////////////////////////////
bool TransformEqualsMatcher::match(const sf::Transform &actual) const {
  // IMPORTANT: Catch2 may reuse matcher objects between assertions.
  // Clear previous mismatch text so stale output can't leak into this run.
  m_mismatch_description.clear();

  // SFML transform is exposed as 16 floats (4x4 matrix, flattened).
  const float *expected_matrix = m_expected.getMatrix();
  const float *actual_matrix = actual.getMatrix();

  // Track which indices mismatch so we can highlight only those values.
  std::array<bool, 16> mismatch_test{};
  mismatch_test.fill(false);

  // Use an explicit boolean for pass/fail status.
  // This is more reliable than inferring from whether a string is empty.
  bool has_mismatch = false;

  // Compare every matrix element with epsilon tolerance.
  for (size_t i = 0; i < 16; ++i) {
    const float diff = std::abs(expected_matrix[i] - actual_matrix[i]);
    if (diff >= m_epsilon) {
      mismatch_test[i] = true;
      has_mismatch = true;
    }
  }

  // Fast path: no mismatches => matcher passes.
  if (!has_mismatch) {
    return true;
  }

  // Build ONE authoritative failure message.
  // Avoid std::cout debug prints to prevent interleaved/misaligned output.
  std::ostringstream oss;

  // Force consistent numeric format for readability:
  // e.g. 0.000001 instead of sometimes showing 1e-06.
  oss << std::fixed << std::setprecision(6);

  oss << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed() << "Transform mismatch:\n";

  // Print ACTUAL matrix with mismatches highlighted.
  oss << conmat::Indent(2) << "actual:\n";
  oss << conmat::Indent(3);
  for (size_t i = 0; i < 16; ++i) {
    // Keep 4x4 visual layout.
    if (i % 4 == 0 && i != 0) {
      oss << "\n" << conmat::Indent(3);
    }

    if (mismatch_test[i]) {
      oss << conmat::Colorize(actual_matrix[i], conmat::Color::Red) << " ";
    } else {
      oss << actual_matrix[i] << " ";
    }
  }

  // Print EXPECTED matrix with corresponding mismatches highlighted.
  oss << "\n" << conmat::Indent(2) << "expected:\n";
  oss << conmat::Indent(3);
  for (size_t i = 0; i < 16; ++i) {
    if (i % 4 == 0 && i != 0) {
      oss << "\n" << conmat::Indent(3);
    }

    if (mismatch_test[i]) {
      oss << conmat::Colorize(expected_matrix[i], conmat::Color::Blue) << " ";
    } else {
      oss << expected_matrix[i] << " ";
    }
  }

  // Optional detail section: exact per-index diff data in same stream.
  oss << "\n"
      << conmat::Indent(2) << "diffs >= epsilon(" << m_epsilon << "):\n";
  for (size_t i = 0; i < 16; ++i) {
    if (mismatch_test[i]) {
      const float diff = std::abs(expected_matrix[i] - actual_matrix[i]);
      oss << conmat::Indent(3) << "idx " << i
          << " expected=" << expected_matrix[i]
          << " actual=" << actual_matrix[i] << " diff=" << diff << "\n";
    }
  }

  // Store final message for describe()/failure reporting.
  m_mismatch_description = oss.str();

  // There were mismatches, so matcher fails.
  return false;
}

/////////////////////////////////////////////////
std::string TransformEqualsMatcher::describe() const {
  std::ostringstream oss;

  if (m_mismatch_description.empty()) {
    oss << conmat::Header(conmat::TestPassed() + " sf::Transform Match:", 3)
        << "\n";
    oss << conmat::Indent(1) << "Transform: \n";
    const float *expected_matrix = m_expected.getMatrix();
    for (size_t i = 0; i < 16; ++i) {
      if (i % 4 == 0 && i != 0) {
        oss << "\n" << conmat::Indent(2);
      }
      oss << expected_matrix[i] << " ";
    }
    oss << "\n";
  } else {
    oss << m_mismatch_description;
  }

  return oss.str();
}
} // namespace steamrot::tests
