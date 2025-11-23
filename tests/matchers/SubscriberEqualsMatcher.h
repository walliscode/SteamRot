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
#include <sstream>
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

  /////////////////////////////////////////////////
  /// @brief Get the name of the EventData variant type by index
  ///
  /// @param index Index of the variant type
  /// @return Name of the variant type
  /////////////////////////////////////////////////
  std::string GetNameForEventDataIndex(size_t index) const;

  /////////////////////////////////////////////////
  /// @brief Compare EventData variants
  ///
  /// @param actual_data Actual event data
  /// @param expected_data Expected event data
  /// @param oss Output stream for mismatch description
  /// @return true if data matches, false otherwise
  /////////////////////////////////////////////////
  bool CompareEventData(const EventData &actual_data,
                        const EventData &expected_data,
                        std::ostringstream &oss) const;

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
