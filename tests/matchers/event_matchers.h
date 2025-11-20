/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matchers for EventPacket and EventBus comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "EventHandler.h"
#include "EventPacket.h"
#include "UserInputBitset.h"
#include "events_generated.h"
#include "test_context.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <conmat.h>
#include <optional>
#include <sstream>
#include <string>
namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class EventPacket specific matcher for detailed comparison
/////////////////////////////////////////////////
class EventPacketEqualsMatcher
    : public Catch::Matchers::MatcherBase<EventPacket> {
private:
  const EventPacket &m_expected;
  mutable std::string m_mismatch_description;

  ///////////////////////////////////////////////
  /// @brief Get the name of the EventData variant type by index
  /// @param index Index of the variant type
  /// @return Name of the variant type
  ///////////////////////////////////////////////
  std::string GetNameForEventDataIndex(size_t index) const {
    switch (index) {
    case 0:
      return "std::monostate";
    case 1:
      return "UserInputBitset";
    case 2:
      return "SceneChangePacket";
    case 3:
      return "UIElementName";
    default:
      return "Unknown";
    }
  }
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
                        std::ostringstream &oss) const {

    // Check if variant types match and return early if they don't
    if (actual_data.index() != expected_data.index()) {
      oss << conmat::TestFailed()
          << "m_event_data variant type differs:" << "\n";
      oss << "\t"
          << "actual type = "
          << conmat::Colorize(GetNameForEventDataIndex(actual_data.index()),
                              conmat::Color::Red)
          << "\n";
      oss << "\t"
          << "expected type = "
          << conmat::Colorize(GetNameForEventDataIndex(expected_data.index()),
                              conmat::Color::Blue)
          << "\n";

      return false;
    }

    // Compare based on variant type
    if (std::holds_alternative<std::monostate>(actual_data)) {

      // Both are monostate, they match
      return true;

    } else if (std::holds_alternative<UserInputBitset>(actual_data)) {
      const auto &actual_bitset = std::get<UserInputBitset>(actual_data);
      const auto &expected_bitset = std::get<UserInputBitset>(expected_data);

      // compare bitset values
      if (actual_bitset != expected_bitset) {
        oss << conmat::TestFailed() << "m_event_data UserInputBitset:" << "\n";
        oss << "\t"
            << "actual = "
            << conmat::Colorize(actual_bitset.to_string(), conmat::Color::Red)
            << "\n";
        oss << "\t"
            << "expected = "
            << conmat::Colorize(expected_bitset.to_string(),
                                conmat::Color::Blue)
            << "\n";
        return false;
      }
    } else if (std::holds_alternative<SceneChangePacket>(actual_data)) {
      const auto &actual_packet = std::get<SceneChangePacket>(actual_data);
      const auto &expected_packet = std::get<SceneChangePacket>(expected_data);

      // UUID is optional, compare presence first
      if (actual_packet.first.has_value() !=
          expected_packet.first.has_value()) {

        oss << conmat::TestFailed()
            << "m_event_data SceneChangePacket UUID presence differs:" << "\n";
        oss << "\t"
            << "actual has value = "
            << conmat::Colorize(actual_packet.first.has_value(),
                                conmat::Color::Red)
            << "\n";
        oss << "\t"
            << "expected has value = "
            << conmat::Colorize(expected_packet.first.has_value(),
                                conmat::Color::Blue)
            << "\n";
        return false;
      } else {

        // compare values if both have value and that it is true
        if (actual_packet.first.has_value() &&
            actual_packet.first.value() != expected_packet.first.value()) {

          oss << conmat::TestFailed()
              << "m_event_data SceneChangePacket UUID differs:" << "\n";
          oss << "\t"
              << "actual = "
              << conmat::Colorize(actual_packet.first.value(),
                                  conmat::Color::Red)
              << "\n";
          oss << "\t"
              << "expected = "
              << conmat::Colorize(expected_packet.first.value(),
                                  conmat::Color::Blue)
              << "\n";
          return false;
        }
      }

      // Compare SceneType
      if (actual_packet.second != expected_packet.second) {
        oss << conmat::TestFailed()
            << "m_event_data SceneChangePacket SceneType differs:" << "\n";
        oss << "\t"
            << "actual = "
            << conmat::Colorize(EnumNameSceneType(actual_packet.second),
                                conmat::Color::Red)
            << "\n";
        oss << "\t"
            << "expected = "
            << conmat::Colorize(EnumNameSceneType(expected_packet.second),
                                conmat::Color::Blue)
            << "\n";
        return false;
      }

    } else if (std::holds_alternative<UIElementName>(actual_data)) {
      const auto &actual_name = std::get<UIElementName>(actual_data);
      const auto &expected_name = std::get<UIElementName>(expected_data);

      if (actual_name != expected_name) {
        oss << conmat::TestFailed() << "m_event_data UIElementName differs:"
            << "\n";
        oss << "\t"
            << "actual = " << conmat::Colorize(actual_name, conmat::Color::Red)
            << "\n";
        oss << "\t"
            << "expected = "
            << conmat::Colorize(expected_name, conmat::Color::Blue) << "\n";
        return false;
      }
    }

    return true;
  }

public:
  explicit EventPacketEqualsMatcher(const EventPacket &expected)
      : m_expected(expected) {}

  bool match(const EventPacket &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    // Compare m_event_type
    if (actual.m_event_type != m_expected.m_event_type) {
      oss << conmat::TestFailed() << "m_event_type:" << "\n";
      oss << "\t"
          << "actual = "
          << conmat::Colorize(EnumNameEventType(actual.m_event_type),
                              conmat::Color::Red)
          << "\n";
      oss << "\t"
          << "expected = "
          << conmat::Colorize(EnumNameEventType(m_expected.m_event_type),
                              conmat::Color::Blue)
          << "\n";
    }

    // Compare m_event_data
    if (!CompareEventData(actual.m_event_data, m_expected.m_event_data, oss)) {
      // Mismatch description already added by CompareEventData
    }

    // Compare event_id
    if (actual.event_id != m_expected.event_id) {
      oss << conmat::TestFailed() << "event_id differs; " << "\n";
      oss << "\t"
          << "actual = " << actual.event_id << "\n";
      oss << "\t"
          << "expected = " << m_expected.event_id << "\n";
    }

    // Compare source_id
    if (actual.source_id != m_expected.source_id) {

      oss << conmat::TestFailed() << "source_id differs:" << "\n";
      oss << "\t"
          << "actual = "
          << conmat::Colorize(actual.source_id, conmat::Color::Red) << "\n";
      oss << "\t"
          << "expected = "
          << conmat::Colorize(m_expected.source_id, conmat::Color::Blue)
          << "\n";
    }

    // Compare event_lifetime
    if (actual.event_lifetime != m_expected.event_lifetime) {
      oss << conmat::TestFailed() << "event_lifetime:" << "\n";
      oss << "\t"
          << "actual = "
          << conmat::Colorize(actual.event_lifetime, conmat::Color::Red)
          << "\n";
      oss << "\t"
          << "expected = "
          << conmat::Colorize(m_expected.event_lifetime, conmat::Color::Blue)
          << "\n";
    }

    m_mismatch_description = oss.str();
    return m_mismatch_description.empty();
  }

  std::string describe() const override {

    // if mismatch description is empty then we can assume the test passed
    if (m_mismatch_description.empty()) {

      std::ostringstream oss;
      oss << conmat::Divider("-", 40) << "\n";
      oss << conmat::TestPassed() << "EventPacket Match" << "\n";
      oss << conmat::Divider("-", 40) << "\n";
      return oss.str();
    } else {

      std::ostringstream oss;
      oss << conmat::Divider("-", 40) << "\n";
      oss << conmat::TestFailed() << "EventPacket Match: " << "\n";
      oss << m_mismatch_description << "\n";
      oss << conmat::Divider("-", 40) << "\n";

      return oss.str();
    }
  }
};

/////////////////////////////////////////////////
/// @brief Helper function to create EventPacketEqualsMatcher
///
/// @param expected The expected EventPacket
/// @return EventPacketEqualsMatcher instance
/////////////////////////////////////////////////
inline EventPacketEqualsMatcher EqualsEventPacket(const EventPacket &expected) {
  return EventPacketEqualsMatcher(expected);
}

/////////////////////////////////////////////////
/// @class EventBus specific matcher for detailed comparison
/////////////////////////////////////////////////
class EventBusEqualsMatcher : public Catch::Matchers::MatcherBase<EventBus> {
private:
  const EventBus &m_expected;
  mutable std::string m_mismatch_description;
  std::optional<TestContext> m_context;

public:
  explicit EventBusEqualsMatcher(const EventBus &expected)
      : m_expected(expected), m_context(std::nullopt) {}

  ////////////////////////////////////////////////////////////
  /// @brief Constructor with TestContext
  ///
  /// @param expected The expected EventBus
  /// @param context Test context with metadata and tick information
  ////////////////////////////////////////////////////////////
  EventBusEqualsMatcher(const EventBus &expected, const TestContext &context)
      : m_expected(expected), m_context(context) {}

  bool match(const EventBus &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    // Return early if sizes differ
    if (actual.size() != m_expected.size()) {
      oss << conmat::TestFailed() << "EventBus size mismatch:" << "\n";
      oss << "\t"
          << "actual size = "
          << conmat::Colorize(actual.size(), conmat::Color::Red) << "\n";
      oss << "\t"
          << "expected size = "
          << conmat::Colorize(m_expected.size(), conmat::Color::Blue) << "\n";
      m_mismatch_description = oss.str();
      return false;
    }

    // Compare each EventPacket
    for (size_t i = 0; i < actual.size(); ++i) {
      EventPacketEqualsMatcher packet_matcher(m_expected[i]);
      if (!packet_matcher.match(actual[i])) {
        oss << "EventPacket at index " << i
            << " differs: " << packet_matcher.describe() << "; ";
      }
    }

    m_mismatch_description = oss.str();
    return m_mismatch_description.empty();
  }

  std::string describe() const override {
    if (m_mismatch_description.empty()) {

      std::ostringstream oss;
      oss << conmat::Colorize("[PASSED] ", conmat::Color::Green)
          << "EventBus Match" << "\n";
      oss << conmat::Divider("=", 40) << "\n";
      return oss.str();
    } else {

      std::ostringstream oss;
      oss << conmat::Colorize("[FAILED] ", conmat::Color::Red)
          << "EventBus Match " << "\n";
      oss << conmat::Divider("-", 40) << "\n";
      oss << m_mismatch_description << "\n";
      oss << conmat::Divider("=", 40) << "\n";

      return oss.str();
    }
  }
};

/////////////////////////////////////////////////
/// @brief Helper function to create EventBusEqualsMatcher
///
/// @param expected The expected EventBus
/// @return EventBusEqualsMatcher instance
/////////////////////////////////////////////////
inline EventBusEqualsMatcher EqualsEventBus(const EventBus &expected) {
  return EventBusEqualsMatcher(expected);
}

/////////////////////////////////////////////////
/// @brief Helper function to create EventBusEqualsMatcher with context
///
/// @param expected The expected EventBus
/// @param context Test context with metadata and tick information
/// @return EventBusEqualsMatcher instance
/////////////////////////////////////////////////
inline EventBusEqualsMatcher EqualsEventBus(const EventBus &expected,
                                            const TestContext &context) {
  return EventBusEqualsMatcher(expected, context);
}

} // namespace steamrot::tests
