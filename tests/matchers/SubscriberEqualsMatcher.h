/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for Subscriber comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "Subscriber.h"
#include <catch2/matchers/catch_matchers.hpp>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class SubscriberEqualsMatcher
/// @brief Subscriber specific matcher for detailed comparison
/////////////////////////////////////////////////
class SubscriberEqualsMatcher
    : public Catch::Matchers::MatcherBase<Subscriber> {
private:
  const Subscriber &m_expected;
  mutable std::string m_mismatch_description;



public:
  /////////////////////////////////////////////////
  /// @brief Constructor for SubscriberEqualsMatcher
  ///
  /// @param expected Expected Subscriber object to compare against
  /////////////////////////////////////////////////
  explicit SubscriberEqualsMatcher(const Subscriber &expected);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual Subscriber with expected
  ///
  /// @param actual Subscriber object to compare
  /// @return Whether the actual Subscriber matches the expected
  /////////////////////////////////////////////////
  bool match(const Subscriber &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  ///
  /// @return Formatted description string
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create SubscriberEqualsMatcher
///
/// @param expected Expected Subscriber object
/// @return SubscriberEqualsMatcher instance
/////////////////////////////////////////////////
inline SubscriberEqualsMatcher EqualsSubscriber(const Subscriber &expected) {
  return SubscriberEqualsMatcher(expected);
}

} // namespace steamrot::tests
