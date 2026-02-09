/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for sf::Vector2f comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Vector2fEqualsMatcher.h"
#include "conmat.h"
#include <cmath>
#include <sstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
Vector2fEqualsMatcher::Vector2fEqualsMatcher(const sf::Vector2f &expected,
                                             float epsilon)
    : m_expected(expected), m_epsilon(epsilon) {}

/////////////////////////////////////////////////
bool Vector2fEqualsMatcher::match(const sf::Vector2f &actual) const {
  m_mismatch_description.clear();

  bool x_equal = std::abs(actual.x - m_expected.x) < m_epsilon;
  bool y_equal = std::abs(actual.y - m_expected.y) < m_epsilon;

  if (!x_equal || !y_equal) {
    std::ostringstream oss;

    if (!x_equal) {
      oss << conmat::Indent(1) << conmat::TestFailed() << "x component:"
          << "\n";
      oss << conmat::Indent(2)
          << "actual: " << conmat::Colorize(actual.x, conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(m_expected.x, conmat::Color::Blue) << "\n";
      oss << conmat::Indent(2) << "difference: "
          << conmat::Colorize(std::abs(actual.x - m_expected.x),
                              conmat::Color::Yellow)
          << " (epsilon: " << m_epsilon << ")" << "\n";
    }

    if (!y_equal) {
      oss << conmat::Indent(1) << conmat::TestFailed() << "y component:"
          << "\n";
      oss << conmat::Indent(2)
          << "actual: " << conmat::Colorize(actual.y, conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(m_expected.y, conmat::Color::Blue) << "\n";
      oss << conmat::Indent(2) << "difference: "
          << conmat::Colorize(std::abs(actual.y - m_expected.y),
                              conmat::Color::Yellow)
          << " (epsilon: " << m_epsilon << ")" << "\n";
    }

    m_mismatch_description = oss.str();
  }

  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////
std::string Vector2fEqualsMatcher::describe() const {
  std::ostringstream oss;

  if (m_mismatch_description.empty()) {
    oss << conmat::Header(conmat::TestPassed() + " sf::Vector2f Match:", 3)
        << "\n";
    oss << conmat::Indent(1) << "Vector: (" << m_expected.x << ", "
        << m_expected.y << ")" << "\n";
  } else {
    oss << conmat::Header(conmat::TestFailed() + " sf::Vector2f Match:", 3)
        << "\n";
    oss << m_mismatch_description;
  }

  return oss.str();
}

} // namespace steamrot::tests
