/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for UIElement comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "UIElement.h"
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class UIElementEqualsMatcher
/// @brief Custom Catch2 matcher for comparing UIElement objects recursively
///
/// Provides detailed output when UIElements don't match, including
/// child element comparison
/////////////////////////////////////////////////
class UIElementEqualsMatcher : public Catch::Matchers::MatcherBase<UIElement> {
private:
  const UIElement &m_expected;
  mutable std::string m_mismatch_description;

  /////////////////////////////////////////////////
  /// @brief Compare two sf::Vector2f values with epsilon tolerance
  ///
  /// @param actual Actual vector
  /// @param expected Expected vector
  /// @param epsilon Tolerance for floating-point comparison
  /// @return true if vectors are equal within tolerance
  /////////////////////////////////////////////////
  bool VectorsEqual(const sf::Vector2f &actual, const sf::Vector2f &expected,
                    float epsilon = 0.0001f) const;

  /////////////////////////////////////////////////
  /// @brief Compare subscription pointers
  ///
  /// @param actual Actual subscription
  /// @param expected Expected subscription
  /// @return true if subscriptions match
  /////////////////////////////////////////////////
  bool SubscriptionsEqual(const std::shared_ptr<Subscriber> &actual,
                          const std::shared_ptr<Subscriber> &expected) const;

  /////////////////////////////////////////////////
  /// @brief Compare response_events vectors
  ///
  /// @param actual Actual response_events
  /// @param expected Expected response_events
  /// @return true if response_events match
  /////////////////////////////////////////////////
  bool ResponseEventsEqual(const std::vector<EventPacket> &actual,
                           const std::vector<EventPacket> &expected) const;

  /////////////////////////////////////////////////
  /// @brief Compare derived type-specific fields
  ///
  /// @param actual Actual UIElement
  /// @param expected Expected UIElement
  /// @param oss Output string stream for mismatch description
  /// @return true if derived fields match
  /////////////////////////////////////////////////
  bool CompareDerivedFields(const UIElement &actual, const UIElement &expected,
                            std::ostringstream &oss) const;

  /////////////////////////////////////////////////
  /// @brief Recursively compare child elements
  ///
  /// @param actual Actual UIElement
  /// @param expected Expected UIElement
  /// @param oss Output string stream for mismatch description
  /// @param depth Current recursion depth for readable output
  /// @return true if all children match
  /////////////////////////////////////////////////
  bool CompareChildren(const UIElement &actual, const UIElement &expected,
                       std::ostringstream &oss, int depth = 0) const;

  /////////////////////////////////////////////////
  /// @brief Compare all fields of two UIElements
  ///
  /// @param actual Actual UIElement
  /// @param expected Expected UIElement
  /// @param output_oss Output string stream for mismatch description
  /// @param depth Current recursion depth for readable output
  /// @return true if all fields match
  /////////////////////////////////////////////////
  bool CompareUIElements(const UIElement &actual, const UIElement &expected,
                         std::ostringstream &output_oss, int depth = 0) const;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for UIElementEqualsMatcher
  ///
  /// @param expected Expected UIElement object to compare against
  /////////////////////////////////////////////////
  explicit UIElementEqualsMatcher(const UIElement &expected);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual UIElement with expected
  ///
  /// @param actual UIElement object to compare
  /// @return Whether the actual UIElement matches the expected
  /////////////////////////////////////////////////
  bool match(const UIElement &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  ///
  /// @return Formatted description string
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create UIElementEqualsMatcher
///
/// @param expected Expected UIElement object
/// @return UIElementEqualsMatcher instance
/////////////////////////////////////////////////
inline UIElementEqualsMatcher EqualsUIElement(const UIElement &expected) {
  return UIElementEqualsMatcher(expected);
}

} // namespace steamrot::tests
