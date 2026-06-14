/////////////////////////////////////////////////
/// @file
/// @brief Implementation of ColorEqualsMatcher for comparing sf::Color objects
/// in Catch2 tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ColorEqualsMatcher.h"
#include "conmat.h"
#include <sstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
ColorEqualsMatcher::ColorEqualsMatcher(const sf::Color &expected)
    : m_expected(expected) {}

/////////////////////////////////////////////////
bool ColorEqualsMatcher::match(const sf::Color &actual) const {
  m_mismatch_description.clear();

  std::ostringstream oss;

  if (actual != m_expected) {
    oss << conmat::Indent(1) << conmat::TestFailed() << "\n";
    oss << conmat::Indent(2)
        << "actual.r: "
        << conmat::Colorize(static_cast<int>(actual.r), conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2)
        << "expected.r: "
        << conmat::Colorize(static_cast<int>(m_expected.r), conmat::Color::Blue)
        << "\n";
    oss << conmat ::Indent(2)
        << "actual.g: "
        << conmat::Colorize(static_cast<int>(actual.g), conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2)
        << "expected.g: "
        << conmat::Colorize(static_cast<int>(m_expected.g), conmat::Color::Blue)
        << "\n";
    oss << conmat::Indent(2)
        << "actual.b: "
        << conmat::Colorize(static_cast<int>(actual.b), conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2)
        << "expected.b: "
        << conmat::Colorize(static_cast<int>(m_expected.b), conmat::Color::Blue)
        << "\n";
    oss << conmat::Indent(2)
        << "actual.a: "
        << conmat::Colorize(static_cast<int>(actual.a), conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2)
        << "expected.a: "
        << conmat::Colorize(static_cast<int>(m_expected.a), conmat::Color::Blue)
        << "\n";
  }

  m_mismatch_description = oss.str();

  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////

std::string ColorEqualsMatcher::describe() const {
  std::ostringstream oss;

  if (m_mismatch_description.empty()) {
    oss << conmat::Header(conmat::TestPassed() + " sf::Color Match", 3) << "\n";
  } else {
    oss << conmat::Header(conmat::TestFailed() + " sf::Color Match:", 3)
        << "\n";
    oss << m_mismatch_description;
  }

  return oss.str();
}

} // namespace steamrot::tests
