/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for EventBus comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "EventHandler.h"
#include "EventPacket.h"
#include "event_packet_matchers.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class EventBusEqualsMatcher
/// @brief Custom Catch2 matcher for comparing EventBus (vector<EventPacket>) objects
///
/// Provides detailed output when EventBus objects don't match,
/// including size mismatches and individual EventPacket differences
/////////////////////////////////////////////////
class EventBusEqualsMatcher : public Catch::Matchers::MatcherBase<EventBus> {
private:
  const EventBus &m_expected;
  mutable std::string m_mismatch_description;
  std::string m_test_metadata;

public:
  explicit EventBusEqualsMatcher(const EventBus &expected)
      : m_expected(expected), m_test_metadata("") {}

  explicit EventBusEqualsMatcher(const EventBus &expected,
                                 const std::string &test_metadata)
      : m_expected(expected), m_test_metadata(test_metadata) {}

  ////////////////////////////////////////////////////////////
  /// @brief Check if the EventBus matches
  ///
  /// @param actual The actual EventBus to compare
  /// @return true if EventBus objects match, false otherwise
  ////////////////////////////////////////////////////////////
  bool match(const EventBus &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    // First check if sizes match
    if (actual.size() != m_expected.size()) {
      oss << "EventBus size differs: actual=" << actual.size()
          << ", expected=" << m_expected.size() << "; ";
      m_mismatch_description = oss.str();
      return false;
    }

    // Compare each EventPacket in the EventBus
    for (size_t i = 0; i < m_expected.size(); ++i) {
      EventPacketEqualsMatcher packet_matcher(m_expected[i]);
      if (!packet_matcher.match(actual[i])) {
        oss << "EventPacket at index " << i << ": "
            << packet_matcher.describe() << "; ";
      }
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
      oss << "equals EventBus with " << m_expected.size() << " events";
      return oss.str();
    }

    std::string result = "EventBus mismatch: " + m_mismatch_description;
    if (!m_test_metadata.empty()) {
      result = m_test_metadata + "\n" + result;
    }
    return result;
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
/// @brief Helper function to create EventBusEqualsMatcher with test metadata
///
/// @param expected The expected EventBus
/// @param test_metadata Test metadata string to include in error messages
/// @return EventBusEqualsMatcher instance
/////////////////////////////////////////////////
inline EventBusEqualsMatcher EqualsEventBus(const EventBus &expected,
                                            const std::string &test_metadata) {
  return EventBusEqualsMatcher(expected, test_metadata);
}

} // namespace steamrot::tests
