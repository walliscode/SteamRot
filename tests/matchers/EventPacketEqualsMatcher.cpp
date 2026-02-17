/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for EventPacket comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPacketEqualsMatcher.h"
#include "EventPacket.h"
#include "EventPayloadEqualsMatcher.h"
#include "conmat.h"
#include "magic_enum/magic_enum.hpp"

namespace steamrot::tests {

/////////////////////////////////////////////////
bool EventPacketEqualsMatcher::CompareEventPayload(
    const EventPayload &actual_payload, const EventPayload &expected_payload,
    std::ostringstream &oss) const {

  EventPayloadEqualsMatcher payload_matcher(expected_payload);
  if (!payload_matcher.match(actual_payload)) {
    oss << payload_matcher.describe();
    return false;
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

  // Compare type
  if (actual.type != m_expected.type) {
    oss << conmat::TestFailed() << "type:" << "\n";
    oss << "\t"
        << "actual = "
        << conmat::Colorize(magic_enum::enum_name(actual.type),
                            conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected = "
        << conmat::Colorize(magic_enum::enum_name(m_expected.type),
                            conmat::Color::Blue)
        << "\n";
  }

  // Compare payload
  if (!CompareEventPayload(actual.payload, m_expected.payload, oss)) {
    // Mismatch already added to oss by CompareEventPayload
  }

  // Compare context.lifetime
  if (actual.context.lifetime != m_expected.context.lifetime) {
    oss << conmat::TestFailed() << "context.lifetime:" << "\n";
    oss << "\t"
        << "actual: "
        << conmat::Colorize(static_cast<int>(actual.context.lifetime),
                            conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected: "
        << conmat::Colorize(static_cast<int>(m_expected.context.lifetime),
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
