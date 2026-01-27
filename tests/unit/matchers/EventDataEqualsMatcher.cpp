/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for EventData comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventDataEqualsMatcher.h"
#include "conmat.h"

namespace steamrot::tests {
/////////////////////////////////////////////////
EventDataEqualsMatcher::EventDataEqualsMatcher(const EventData &expected)
    : m_expected(expected) {}

/////////////////////////////////////////////////
std::string
EventDataEqualsMatcher::GetNameForEventDataIndex(size_t index) const {
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
bool EventDataEqualsMatcher::match(const EventData &actual_data) const {
  m_mismatch_description.clear();
  std::ostringstream oss;
  // Check if variant types match and return early if they don't
  if (actual_data.index() != m_expected.index()) {
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "EventData variant type differs:" << "\n";
    oss << conmat::Indent(2) << "actual: "
        << conmat::Colorize(GetNameForEventDataIndex(actual_data.index()),
                            conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(GetNameForEventDataIndex(m_expected.index()),
                            conmat::Color::Blue)
        << "\n";
    m_mismatch_description = oss.str();
    // return early since types differ
    return false;
  }

  // Compare based on variant type
  if (std::holds_alternative<std::monostate>(actual_data)) {

    // Both are monostate, they match

  } else if (std::holds_alternative<UserInputBitset>(actual_data)) {
    const auto &actual_bitset = std::get<UserInputBitset>(actual_data);
    const auto &expected_bitset = std::get<UserInputBitset>(m_expected);

    // compare bitset values
    if (actual_bitset != expected_bitset) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "m_event_data UserInputBitset differs:" << "\n";
      oss << conmat::Indent(2)
          << "actual: " << conmat::Colorize(actual_bitset, conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(expected_bitset, conmat::Color::Blue) << "\n";
    }

  } else if (std::holds_alternative<SceneChangePacket>(actual_data)) {
    const auto &actual_packet = std::get<SceneChangePacket>(actual_data);
    const auto &expected_packet = std::get<SceneChangePacket>(m_expected);

    // UUID is optional, compare presence first
    if (actual_packet.first.has_value() != expected_packet.first.has_value()) {

      oss << conmat::Indent(1) << conmat::TestFailed()
          << "m_event_data SceneChangePacket UUID presence differs:" << "\n";
      oss << conmat::Indent(2) << "actual: "
          << conmat::Colorize(actual_packet.first.has_value(),
                              conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected has value: "
          << conmat::Colorize(expected_packet.first.has_value(),
                              conmat::Color::Blue)
          << "\n";

    } else {

      // compare values if both have value and that it is true
      if (actual_packet.first.has_value() &&
          actual_packet.first.value() != expected_packet.first.value()) {

        oss << conmat::Indent(1) << conmat::TestFailed()
            << "m_event_data SceneChangePacket UUID differs:" << "\n";
        oss << conmat::Indent(2) << "actual: "
            << conmat::Colorize(actual_packet.first.value(), conmat::Color::Red)
            << "\n";
        oss << conmat::Indent(2) << "expected: "
            << conmat::Colorize(expected_packet.first.value(),
                                conmat::Color::Blue)
            << "\n";
      }
    }

    // Compare SceneType
    if (actual_packet.second != expected_packet.second) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "m_event_data SceneChangePacket SceneType differs:" << "\n";
      oss << conmat::Indent(2) << "actual: "
          << conmat::Colorize(EnumNameSceneType(actual_packet.second),
                              conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(EnumNameSceneType(expected_packet.second),
                              conmat::Color::Blue)
          << "\n";
    }

  } else if (std::holds_alternative<UserInterfaceName>(actual_data)) {
    const auto &actual_name = std::get<UserInterfaceName>(actual_data);
    const auto &expected_name = std::get<UserInterfaceName>(m_expected);

    if (actual_name != expected_name) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "m_event_data UserInterfaceName differs:"
          << "\n";
      oss << conmat::Indent(2)
          << "actual: " << conmat::Colorize(actual_name, conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(expected_name, conmat::Color::Blue) << "\n";
    }
  }
  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////
std::string EventDataEqualsMatcher::describe() const {
  if (m_mismatch_description.empty()) {
    return conmat::Header(conmat::TestPassed() + "EventData Match:", 3) + "\n";
  } else {
    return conmat::Header(conmat::TestFailed() + "EventData Mismatch:", 3) +
           "\n" + m_mismatch_description;
  }
}

} // namespace steamrot::tests
