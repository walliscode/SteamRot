/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for Subscriber comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberEqualsMatcher.h"
#include "Subscriber.h"
#include "conmat.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::string
SubscriberEqualsMatcher::GetNameForEventDataIndex(size_t index) const {
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
bool SubscriberEqualsMatcher::CompareEventData(const EventData &actual_data,
                                               const EventData &expected_data,
                                               std::ostringstream &oss) const {

  // Check if variant types match and return early if they don't
  if (actual_data.index() != expected_data.index()) {
    oss << conmat::TestFailed() << "EventData variant type differs:" << "\n";
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
      oss << conmat::TestFailed() << "EventData UserInputBitset:" << "\n";
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
          << "EventData SceneChangePacket UUID presence differs:" << "\n";
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
            << "EventData SceneChangePacket UUID differs:" << "\n";
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
          << "EventData SceneChangePacket SceneType differs:" << "\n";
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
      oss << conmat::TestFailed() << "EventData UserInterfaceName differs:"
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
SubscriberEqualsMatcher::SubscriberEqualsMatcher(const Subscriber &expected)
    : m_expected(expected) {}

/////////////////////////////////////////////////
bool SubscriberEqualsMatcher::match(const Subscriber &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  // Compare m_active
  if (actual.IsActive() != m_expected.IsActive()) {
    oss << conmat::TestFailed() << "IsActive():" << "\n";
    oss << "\t"
        << "actual = "
        << conmat::Colorize(actual.IsActive(), conmat::Color::Red) << "\n";
    oss << "\t"
        << "expected = "
        << conmat::Colorize(m_expected.IsActive(), conmat::Color::Blue)
        << "\n";
  }

  // Compare m_event_type
  if (actual.GetEventType() != m_expected.GetEventType()) {
    oss << conmat::TestFailed() << "GetEventType():" << "\n";
    oss << "\t"
        << "actual = "
        << conmat::Colorize(EnumNameEventType(actual.GetEventType()),
                            conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected = "
        << conmat::Colorize(EnumNameEventType(m_expected.GetEventType()),
                            conmat::Color::Blue)
        << "\n";
  }

  // Compare m_event_data
  CompareEventData(actual.GetEventData(), m_expected.GetEventData(), oss);

  // Compare m_trigger_data
  if (actual.GetTriggerData().has_value() !=
      m_expected.GetTriggerData().has_value()) {
    oss << conmat::TestFailed() << "GetTriggerData() presence differs:" << "\n";
    oss << "\t"
        << "actual has value = "
        << conmat::Colorize(actual.GetTriggerData().has_value(),
                            conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected has value = "
        << conmat::Colorize(m_expected.GetTriggerData().has_value(),
                            conmat::Color::Blue)
        << "\n";
  } else if (actual.GetTriggerData().has_value()) {
    // Both have values, compare them
    CompareEventData(actual.GetTriggerData().value(),
                     m_expected.GetTriggerData().value(), oss);
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////
std::string SubscriberEqualsMatcher::describe() const {

  // if mismatch description is empty then we can assume the test passed
  if (m_mismatch_description.empty()) {

    std::ostringstream oss;
    oss << conmat::Divider("-", 40) << "\n";
    oss << conmat::TestPassed() << "Subscriber Match" << "\n";
    oss << conmat::Divider("-", 40) << "\n";
    return oss.str();
  } else {

    std::ostringstream oss;
    oss << conmat::Divider("-", 40) << "\n";
    oss << conmat::TestFailed() << "Subscriber Match: " << "\n";
    oss << m_mismatch_description << "\n";
    oss << conmat::Divider("-", 40) << "\n";

    return oss.str();
  }
}

} // namespace steamrot::tests
