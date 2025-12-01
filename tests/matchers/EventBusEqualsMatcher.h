/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for EventBus comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "EventHandler.h"
#include <catch2/matchers/catch_matchers.hpp>
#include <optional>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class EventBusEqualsMatcher
/// @brief EventBus specific matcher for detailed comparison
/////////////////////////////////////////////////
class EventBusEqualsMatcher : public Catch::Matchers::MatcherBase<EventBus> {
private:
  const EventBus &m_expected;
  mutable std::string m_mismatch_description;
  std::optional<TestContext> m_context;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for EventBusEqualsMatcher
  ///
  /// @param expected Expected EventBus object to compare against
  /////////////////////////////////////////////////
  explicit EventBusEqualsMatcher(const EventBus &expected);

  /////////////////////////////////////////////////
  /// @brief Constructor with TestContext
  ///
  /// @param expected Expected EventBus object to compare against
  /// @param context Test context with metadata and tick information
  /////////////////////////////////////////////////
  EventBusEqualsMatcher(const EventBus &expected, const TestContext &context);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual EventBus with expected
  ///
  /// @param actual EventBus object to compare
  /// @return Whether the actual EventBus matches the expected
  /////////////////////////////////////////////////
  bool match(const EventBus &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  ///
  /// @return Formatted description string
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create EventBusEqualsMatcher
///
/// @param expected Expected EventBus object
/// @return EventBusEqualsMatcher instance
/////////////////////////////////////////////////
inline EventBusEqualsMatcher EqualsEventBus(const EventBus &expected) {
  return EventBusEqualsMatcher(expected);
}

/////////////////////////////////////////////////
/// @brief Helper function to create EventBusEqualsMatcher with context
///
/// @param expected Expected EventBus object
/// @param context Test context with metadata and tick information
/// @return EventBusEqualsMatcher instance
/////////////////////////////////////////////////
inline EventBusEqualsMatcher EqualsEventBus(const EventBus &expected,
                                            const TestContext &context) {
  return EventBusEqualsMatcher(expected, context);
}

} // namespace steamrot::tests
