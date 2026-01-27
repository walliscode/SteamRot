/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for Subscriber comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberEqualsMatcher.h"
#include "EventDataEqualsMatcher.h"
#include "EventType.h"
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
SubscriberEqualsMatcher::SubscriberEqualsMatcher(const Subscriber &expected)
    : m_expected(expected) {}

/////////////////////////////////////////////////
bool SubscriberEqualsMatcher::match(const Subscriber &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  // Compare m_active
  if (actual.m_active != m_expected.m_active) {
    oss << conmat::TestFailed() << "m_active:" << "\n";
    oss << "\t"
        << "actual = "
        << conmat::Colorize(actual.m_active, conmat::Color::Red) << "\n";
    oss << "\t"
        << "expected = "
        << conmat::Colorize(m_expected.m_active, conmat::Color::Blue) << "\n";
  }

  // Compare m_trigger_event_type
  if (actual.m_trigger_event_type != m_expected.m_trigger_event_type) {
    oss << conmat::TestFailed() << "m_trigger_event_type:" << "\n";
    oss << "\t"
        << "actual = "
        << conmat::Colorize(EnumNameEventType(actual.m_trigger_event_type),
                            conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected = "
        << conmat::Colorize(EnumNameEventType(m_expected.m_trigger_event_type),
                            conmat::Color::Blue)
        << "\n";
  }

  // Compare m_trigger_event_data using EventData matcher but accounting for
  // optional
  if (m_expected.m_trigger_event_data.has_value() &&
      actual.m_trigger_event_data.has_value()) {
    auto trigger_data_matcher =
        EventDataEqualsMatcher(m_expected.m_trigger_event_data.value());
    if (!trigger_data_matcher.match(actual.m_trigger_event_data.value())) {
      oss << conmat::TestFailed() << "m_trigger_event_data differs:" << "\n";
      oss << trigger_data_matcher.describe();
    }
  } else if (m_expected.m_trigger_event_data.has_value() !=
             actual.m_trigger_event_data.has_value()) {
    oss << conmat::TestFailed()
        << "m_trigger_event_data presence differs:" << "\n";
    oss << "\t"
        << "actual has value = "
        << conmat::Colorize(actual.m_trigger_event_data.has_value(),
                            conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected has value = "
        << conmat::Colorize(m_expected.m_trigger_event_data.has_value(),
                            conmat::Color::Blue)
        << "\n";
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
