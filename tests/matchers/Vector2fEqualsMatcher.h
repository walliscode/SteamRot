/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for sf::Vector2f comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/System/Vector2.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class Vector2fEqualsMatcher
/// @brief Matcher for comparing sf::Vector2f with epsilon tolerance
/////////////////////////////////////////////////
class Vector2fEqualsMatcher
    : public Catch::Matchers::MatcherBase<sf::Vector2f> {
private:
  /////////////////////////////////////////////////
  /// @brief Expected vector to compare against
  /////////////////////////////////////////////////
  sf::Vector2f m_expected;

  /////////////////////////////////////////////////
  /// @brief Epsilon tolerance for floating point comparison
  /////////////////////////////////////////////////
  float m_epsilon;

  /////////////////////////////////////////////////
  /// @brief Store mismatch description for reporting
  ///
  /// @note mutable to allow modification in const match method
  /////////////////////////////////////////////////
  mutable std::string m_mismatch_description;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for Vector2fEqualsMatcher
  ///
  /// @param expected Expected sf::Vector2f to compare against
  /// @param epsilon Tolerance for floating point comparison (default: 1e-5f)
  /////////////////////////////////////////////////
  explicit Vector2fEqualsMatcher(const sf::Vector2f &expected,
                                 float epsilon = 1e-5f);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual vector with expected
  ///
  /// @param actual sf::Vector2f to compare
  /// @return Whether the actual vector matches the expected within epsilon
  /////////////////////////////////////////////////
  bool match(const sf::Vector2f &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  ///
  /// @return Formatted description string
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create Vector2fEqualsMatcher
///
/// @param expected Expected sf::Vector2f
/// @param epsilon Tolerance for floating point comparison (default: 1e-5f)
/// @return Vector2fEqualsMatcher instance
/////////////////////////////////////////////////
inline Vector2fEqualsMatcher EqualsVector2f(const sf::Vector2f &expected,
                                            float epsilon = 1e-5f) {
  return Vector2fEqualsMatcher(expected, epsilon);
}

} // namespace steamrot::tests
