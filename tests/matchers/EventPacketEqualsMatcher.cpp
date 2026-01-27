/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for EventPacket comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPacketEqualsMatcher.h"
#include "EventPacket.h"
#include "EventType.h"
#include "conmat.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::string
EventPacketEqualsMatcher::GetNameForEventDataIndex(size_t index) const {
  switch (index) {
  case 0:
    return "std::monostate";
  case 1:
    return "UserInputBitset";
  case 2:
    return "SceneChangePacket";
  case 3:
    return "UserInterfaceName";
  default:
    return "Unknown";
  }
}

/////////////////////////////////////////////////
bool EventPacketEqualsMatcher::CompareEventData(const EventData &actual_data,
                                                const EventData &expected_data,
                                                std::ostringstream &oss) const {

  // Check if variant types match and return early if they don't
  if (actual_data.index() != expected_data.index()) {
    oss << conmat::TestFailed() << "event_data variant type differs:"
        << "\n";
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
      oss << conmat::TestFailed() << "event_data UserInputBitset:" << "\n";
      oss << "\t"
          << "actual = "
          << conmat::Colorize(actual_bitset.to_string(), conmat::Color::Red)
          << "\n";
      oss << "\t"
          << "expected = "
          << conmat::Colorize(expected_bitset.to_string(), conmat::Color::Blue)
          << "\n";
      return false;
    }
  } else if (std::holds_alternative<SceneChangePacket>(actual_data)) {
    const auto &actual_packet = std::get<SceneChangePacket>(actual_data);
    const auto &expected_packet = std::get<SceneChangePacket>(expected_data);

    // UUID is optional, compare presence first
    if (actual_packet.first.has_value() != expected_packet.first.has_value()) {

      oss << conmat::TestFailed()
          << "event_data SceneChangePacket UUID presence differs:" << "\n";
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
            << "event_data SceneChangePacket UUID differs:" << "\n";
        oss << "\t"
            << "actual = "
            << conmat::Colorize(actual_packet.first.value(), conmat::Color::Red)
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
          << "event_data SceneChangePacket SceneType differs:" << "\n";
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

  } else if (std::holds_alternative<UserInterfaceName>(actual_data)) {
    const auto &actual_name = std::get<UserInterfaceName>(actual_data);
    const auto &expected_name = std::get<UserInterfaceName>(expected_data);

    if (actual_name != expected_name) {
      oss << conmat::TestFailed() << "event_data UserInterfaceName differs:"
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

/////////////////////////////////////////////////
EventPacketEqualsMatcher::EventPacketEqualsMatcher(const EventPacket &expected)
    : m_expected(expected) {}

/////////////////////////////////////////////////
bool EventPacketEqualsMatcher::match(const EventPacket &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  // Compare event_type
  if (actual.event_type != m_expected.event_type) {
    oss << conmat::TestFailed() << "event_type:" << "\n";
    oss << "\t"
        << "actual = "
        << conmat::Colorize(EnumNameEventType(actual.event_type),
                            conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected = "
        << conmat::Colorize(EnumNameEventType(m_expected.event_type),
                            conmat::Color::Blue)
        << "\n";
  }

  // Compare event_data
  CompareEventData(actual.event_data, m_expected.event_data, oss);

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
        << "actual = " << conmat::Colorize(actual.source_id, conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected = "
        << conmat::Colorize(m_expected.source_id, conmat::Color::Blue) << "\n";
  }

  // Compare event_lifetime
  if (actual.event_lifetime != m_expected.event_lifetime) {
    oss << conmat::TestFailed() << "event_lifetime:" << "\n";
    oss << "\t"
        << "actual: "
        << conmat::Colorize(static_cast<int>(actual.event_lifetime),
                            conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected: "
        << conmat::Colorize(static_cast<int>(m_expected.event_lifetime),
                            conmat::Color::Blue)
        << "\n";
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////
std::string EventPacketEqualsMatcher::describe() const {

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

} // namespace steamrot::tests
