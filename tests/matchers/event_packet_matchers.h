/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for EventPacket comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "EventPacket.h"
#include "uuid.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>
#include <variant>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class EventPacketEqualsMatcher
/// @brief Custom Catch2 matcher for comparing EventPacket objects
///
/// Provides detailed output when EventPackets don't match
/////////////////////////////////////////////////
class EventPacketEqualsMatcher
    : public Catch::Matchers::MatcherBase<EventPacket> {
private:
  const EventPacket &m_expected;
  mutable std::string m_mismatch_description;

  /////////////////////////////////////////////////
  /// @brief Helper to compare EventData variants
  ///
  /// @param actual The actual EventData
  /// @param expected The expected EventData
  /// @param oss Output stream for mismatch descriptions
  /// @return true if EventData matches, false otherwise
  /////////////////////////////////////////////////
  bool CompareEventData(const EventData &actual, const EventData &expected,
                        std::ostringstream &oss) const {
    if (actual.index() != expected.index()) {
      oss << "m_event_data type index differs: actual=" << actual.index()
          << ", expected=" << expected.index() << "; ";
      return false;
    }

    // Both have same type, now compare values
    if (std::holds_alternative<std::monostate>(actual)) {
      // monostate has no value to compare
      return true;
    } else if (std::holds_alternative<UserInputBitset>(actual)) {
      const auto &actual_bitset = std::get<UserInputBitset>(actual);
      const auto &expected_bitset = std::get<UserInputBitset>(expected);
      if (actual_bitset != expected_bitset) {
        oss << "m_event_data UserInputBitset differs; ";
        return false;
      }
    } else if (std::holds_alternative<SceneChangePacket>(actual)) {
      const auto &actual_packet = std::get<SceneChangePacket>(actual);
      const auto &expected_packet = std::get<SceneChangePacket>(expected);

      // Compare optional uuid
      if (actual_packet.first.has_value() != expected_packet.first.has_value()) {
        oss << "m_event_data SceneChangePacket uuid presence differs; ";
        return false;
      }
      if (actual_packet.first.has_value() &&
          actual_packet.first.value() != expected_packet.first.value()) {
        oss << "m_event_data SceneChangePacket uuid differs; ";
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
    } else if (std::holds_alternative<UIElementName>(actual)) {
      const auto &actual_name = std::get<UIElementName>(actual);
      const auto &expected_name = std::get<UIElementName>(expected);
      if (actual_name != expected_name) {
        oss << "m_event_data UIElementName differs: actual=\"" << actual_name
            << "\", expected=\"" << expected_name << "\"; ";
        return false;
      }
    }

    return true;
  }

public:
  explicit EventPacketEqualsMatcher(const EventPacket &expected)
      : m_expected(expected) {}

  ////////////////////////////////////////////////////////////
  /// @brief Check if the EventPacket matches
  ///
  /// @param actual The actual EventPacket to compare
  /// @return true if EventPackets match, false otherwise
  ////////////////////////////////////////////////////////////
  bool match(const EventPacket &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    // Compare m_event_type
    if (actual.m_event_type != m_expected.m_event_type) {
      oss << "m_event_type differs: actual="
          << static_cast<int>(actual.m_event_type)
          << ", expected=" << static_cast<int>(m_expected.m_event_type) << "; ";
    }

    // Compare m_event_data
    if (!CompareEventData(actual.m_event_data, m_expected.m_event_data, oss)) {
      // Error message already added by CompareEventData
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
      oss << "event_lifetime differs: actual="
          << static_cast<int>(actual.event_lifetime)
          << ", expected=" << static_cast<int>(m_expected.event_lifetime)
          << "; ";
    }

    m_mismatch_description = oss.str();
    return m_mismatch_description.empty();
  }

  ////////////////////////////////////////////////////////////
  /// @brief Describe the matcher
  ///
  /// @return Description string
  ////////////////////////////////////////////////////////////
  std::string describe() const override {
    if (m_mismatch_description.empty()) {
      std::ostringstream oss;
      oss << "equals EventPacket with event_type="
          << static_cast<int>(m_expected.m_event_type)
          << ", event_lifetime=" << static_cast<int>(m_expected.event_lifetime);
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

} // namespace steamrot::tests
