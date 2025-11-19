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
    // Check if variant types match
    if (actual_data.index() != expected_data.index()) {
      oss << "m_event_data variant type differs: actual index="
          << actual_data.index() << ", expected index=" << expected_data.index()
          << "; ";
      return false;
    }

    // Compare based on variant type
    if (std::holds_alternative<std::monostate>(actual_data)) {
      // Both are monostate, they match
      return true;
    } else if (std::holds_alternative<UserInputBitset>(actual_data)) {
      const auto &actual_bitset = std::get<UserInputBitset>(actual_data);
      const auto &expected_bitset = std::get<UserInputBitset>(expected_data);
      if (actual_bitset != expected_bitset) {
        oss << "m_event_data UserInputBitset differs; ";
        return false;
      }
    } else if (std::holds_alternative<SceneChangePacket>(actual_data)) {
      const auto &actual_packet = std::get<SceneChangePacket>(actual_data);
      const auto &expected_packet = std::get<SceneChangePacket>(expected_data);

      // Compare optional UUID
      if (actual_packet.first.has_value() !=
          expected_packet.first.has_value()) {
        oss << "m_event_data SceneChangePacket UUID presence differs; ";
        return false;
      }
      if (actual_packet.first.has_value() &&
          actual_packet.first.value() != expected_packet.first.value()) {
        oss << "m_event_data SceneChangePacket UUID differs; ";
        return false;
      }

      // Compare SceneType
      if (actual_packet.second != expected_packet.second) {
        oss << "m_event_data SceneChangePacket SceneType differs: actual="
            << static_cast<int>(actual_packet.second)
            << ", expected=" << static_cast<int>(expected_packet.second)
            << "; ";
        return false;
      }
    } else if (std::holds_alternative<UIElementName>(actual_data)) {
      const auto &actual_name = std::get<UIElementName>(actual_data);
      const auto &expected_name = std::get<UIElementName>(expected_data);
      if (actual_name != expected_name) {
        oss << "m_event_data UIElementName differs: actual='" << actual_name
            << "', expected='" << expected_name << "'; ";
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
      oss << "m_event_type: actual=" << static_cast<int>(actual.m_event_type)
          << ", expected=" << static_cast<int>(m_expected.m_event_type) << "; ";
    }

    // Compare m_event_data
    if (!CompareEventData(actual.m_event_data, m_expected.m_event_data, oss)) {
      // Mismatch description already added by CompareEventData
    }

    // Compare event_id
    if (actual.event_id != m_expected.event_id) {
      oss << "event_id differs; ";
    }

    // Compare source_id
    if (actual.source_id != m_expected.source_id) {
      oss << "source_id differs; ";
    }

    // Compare event_lifetime
    if (actual.event_lifetime != m_expected.event_lifetime) {
      oss << "event_lifetime: actual="
          << static_cast<int>(actual.event_lifetime)
          << ", expected=" << static_cast<int>(m_expected.event_lifetime)
          << "; ";
    }

    m_mismatch_description = oss.str();
    return m_mismatch_description.empty();
  }

  std::string describe() const override {
    if (m_mismatch_description.empty()) {
      std::ostringstream oss;
      oss << "equals EventPacket(event_type="
          << static_cast<int>(m_expected.m_event_type)
          << ", lifetime=" << static_cast<int>(m_expected.event_lifetime)
          << ")";
      return oss.str();
    }
    return "EventPacket mismatch: " + m_mismatch_description;
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

    // Compare sizes
    if (actual.size() != m_expected.size()) {
      oss << "EventBus size differs: actual=" << actual.size()
          << ", expected=" << m_expected.size() << "; ";
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
      oss << "equals EventBus with " << m_expected.size() << " events";
      return oss.str();
    }

    // Use TestContext formatting if available
    if (m_context.has_value()) {
      return m_context.value().FormatFailureMessage("EventBus",
                                                    m_mismatch_description);
    }

    // Legacy support - manual formatting
    std::ostringstream oss;
    oss << "\n[FAILED] EventBus Comparison";
    oss << "\n" << std::string(60, '=');
    oss << "\n" << std::string(60, '-');

    if (!m_mismatch_description.empty()) {
      oss << "\n  Differences:";
      oss << "\n    * " << m_mismatch_description;
    }

    oss << "\n" << std::string(60, '=');

    return oss.str();
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
