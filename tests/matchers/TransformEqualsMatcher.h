/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for comparing sf::Transform objects
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics/Transform.hpp>
#include <catch2/matchers/catch_matchers.hpp>

namespace steamrot::tests {

class TransformEqualsMatcher
    : public Catch::Matchers::MatcherBase<sf::Transform> {

private:
  sf::Transform m_expected;
  float m_epsilon;
  mutable std::string m_mismatch_description;

public:
  explicit TransformEqualsMatcher(const sf::Transform &expected,
                                  float epsilon = 1e-5f);

  bool match(const sf::Transform &actual) const override;

  std::string describe() const override;
};

inline TransformEqualsMatcher TransformEquals(const sf::Transform &expected,
                                              float epsilon = 1e-5f) {
  return TransformEqualsMatcher(expected, epsilon);
}
} // namespace steamrot::tests
