/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for Subscriber comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberEqualsMatcher.h"
#include "EventPayloadEqualsMatcher.h"
#include "Subscriber.h"
#include "conmat.h"
#include <magic_enum/magic_enum.hpp>

namespace steamrot::tests {

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
        << "actual = " << conmat::Colorize(actual.m_active, conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected = "
        << conmat::Colorize(m_expected.m_active, conmat::Color::Blue) << "\n";
  }

  // Compare event_type
  if (actual.event_type != m_expected.event_type) {
    oss << conmat::TestFailed() << "event_type:" << "\n";
    oss << "\t"
        << "actual = "
        << conmat::Colorize(magic_enum::enum_name(actual.event_type),
                            conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected = "
        << conmat::Colorize(magic_enum::enum_name(m_expected.event_type),
                            conmat::Color::Blue)
        << "\n";
  }

  // Compare filter_payload using EventPayload matcher
  if (m_expected.filter_payload.has_value() &&
      actual.filter_payload.has_value()) {
    auto filter_payload_matcher =
        EventPayloadEqualsMatcher(m_expected.filter_payload.value());
    if (!filter_payload_matcher.match(actual.filter_payload.value())) {
      oss << conmat::TestFailed() << "filter_payload differs:" << "\n";
      oss << filter_payload_matcher.describe();
    }
  } else if (m_expected.filter_payload.has_value() !=
             actual.filter_payload.has_value()) {
    oss << conmat::TestFailed() << "filter_payload presence differs:" << "\n";
    oss << "\t"
        << "actual has value = "
        << conmat::Colorize(actual.filter_payload.has_value(),
                            conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected has value = "
        << conmat::Colorize(m_expected.filter_payload.has_value(),
                            conmat::Color::Blue)
        << "\n";
  }

  // Compare captured_payload using EventPayload matcher
  if (m_expected.captured_payload.has_value() &&
      actual.captured_payload.has_value()) {
    auto captured_payload_matcher =
        EventPayloadEqualsMatcher(m_expected.captured_payload.value());
    if (!captured_payload_matcher.match(actual.captured_payload.value())) {
      oss << conmat::TestFailed() << "captured_payload differs:" << "\n";
      oss << captured_payload_matcher.describe();
    }
  } else if (m_expected.captured_payload.has_value() !=
             actual.captured_payload.has_value()) {
    oss << conmat::TestFailed() << "captured_payload presence differs:" << "\n";
    oss << "\t"
        << "actual has value = "
        << conmat::Colorize(actual.captured_payload.has_value(),
                            conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected has value = "
        << conmat::Colorize(m_expected.captured_payload.has_value(),
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
