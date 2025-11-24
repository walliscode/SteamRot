/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for EventBus comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventBusEqualsMatcher.h"
#include "EventPacketEqualsMatcher.h"
#include "conmat.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
EventBusEqualsMatcher::EventBusEqualsMatcher(const EventBus &expected)
    : m_expected(expected), m_context(std::nullopt) {}

/////////////////////////////////////////////////
EventBusEqualsMatcher::EventBusEqualsMatcher(const EventBus &expected,
                                             const TestContext &context)
    : m_expected(expected), m_context(context) {}

/////////////////////////////////////////////////
bool EventBusEqualsMatcher::match(const EventBus &actual) const {
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

/////////////////////////////////////////////////
std::string EventBusEqualsMatcher::describe() const {
  if (m_mismatch_description.empty()) {

    std::ostringstream oss;
    oss << conmat::Colorize("[PASSED] ", conmat::Color::Green)
        << "EventBus Match" << "\n";
    
    // Include context information if available
    if (m_context.has_value() && m_context->HasContent()) {
      oss << m_context->FormatContextSection();
      oss << "\n";
    }
    
    oss << conmat::Divider("=", 40) << "\n";
    return oss.str();
  } else {

    std::ostringstream oss;
    oss << conmat::Colorize("[FAILED] ", conmat::Color::Red)
        << "EventBus Match " << "\n";
    
    // Include context information if available
    if (m_context.has_value() && m_context->HasContent()) {
      oss << m_context->FormatContextSection();
      oss << "\n";
    }
    
    oss << conmat::Divider("-", 40) << "\n";
    oss << m_mismatch_description << "\n";
    oss << conmat::Divider("=", 40) << "\n";

    return oss.str();
  }
}

} // namespace steamrot::tests
