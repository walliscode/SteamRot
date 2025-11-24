/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for CUIState comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CUIStateEqualsMatcher.h"
#include "SubscriberEqualsMatcher.h"
#include "conmat.h"
#include "matcher_helpers.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::string CUIStateEqualsMatcher::GetComponentName() const {
  return "CUIState";
}

/////////////////////////////////////////////////
CUIStateEqualsMatcher::CUIStateEqualsMatcher(const CUIState &expected)
    : ComponentMatcherBase<CUIState>(expected) {}

/////////////////////////////////////////////////
bool CUIStateEqualsMatcher::match(const CUIState &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  if (actual.m_active != m_expected.m_active) {
    oss << conmat::Indent(1) << conmat::TestFailed() << "m_active:"
        << "\n";

    oss << conmat::Indent(2)
        << "actual: " << conmat::Colorize(actual.m_active, conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(m_expected.m_active, conmat::Color::Blue) << "\n";
  }

  // run comparisons for custom CUIState members
  CompareMapStateToUIVisibility(m_expected.m_state_to_ui_visibility,
                                actual.m_state_to_ui_visibility, oss);
  CompareMapStateValues(m_expected.m_state_values, actual.m_state_values, oss);
  CompareMapStateSubscribers(m_expected.m_state_subscribers,
                             actual.m_state_subscribers, oss);
  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////
void CompareUIVisibilityState(const std::string &state_key,
                              const UIVisibilityState &expected,
                              const UIVisibilityState &actual,
                              std::ostringstream &oss) {
  if (expected.m_ui_indices_on != actual.m_ui_indices_on) {

    oss << conmat::Indent(1) << conmat::TestFailed() << state_key
        << " m_ui_indices_on:"
        << "\n";
    oss << conmat::Indent(2) << "actual: " << expected.m_ui_indices_on << "\n";
    oss << conmat::Indent(2) << "expected: " << actual.m_ui_indices_on << "\n";
  }
  if (expected.m_ui_indices_off != actual.m_ui_indices_off) {
    oss << conmat::Indent(1) << conmat::TestFailed() << state_key
        << " m_ui_indices_off:"
        << "\n";
    oss << conmat::Indent(2)
        << "actual: " << conmat::Colorize("differs", conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2)
        << "expected: " << conmat::Colorize("differs", conmat::Color::Blue)
        << "\n";
  }
}

/////////////////////////////////////////////////
void CompareMapStateToUIVisibility(
    const std::unordered_map<std::string, UIVisibilityState> &expected,
    const std::unordered_map<std::string, UIVisibilityState> &actual,
    std::ostringstream &oss) {

  for (const auto &[key, expected_state] : expected) {
    auto actual_it = actual.find(key);
    if (actual_it == actual.end()) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "Missing key in actual: " << key << "\n";
      continue;
    }
    const auto &actual_state = actual_it->second;
    CompareUIVisibilityState(actual_it->first, expected_state, actual_state,
                             oss);
  }
}

/////////////////////////////////////////////////
void CompareMapStateValues(
    const std::unordered_map<std::string, bool> &expected,
    const std::unordered_map<std::string, bool> &actual,
    std::ostringstream &oss) {
  for (const auto &[key, expected_value] : expected) {
    auto actual_it = actual.find(key);
    if (actual_it == actual.end()) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "Missing key in actual: " << key << "\n";
      continue;
    }
    const auto &actual_value = actual_it->second;
    if (expected_value != actual_value) {
      oss << conmat::Indent(1) << conmat::TestFailed() << "Key: " << key
          << " value mismatch:"
          << "\n";
      oss << conmat::Indent(2)
          << "actual: " << conmat::Colorize(actual_value, conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(expected_value, conmat::Color::Blue) << "\n";
    }
  }
}

/////////////////////////////////////////////////
void CompareMapStateSubscribers(
    const std::unordered_map<
        std::string, std::vector<std::shared_ptr<Subscriber>>> &expected,
    const std::unordered_map<std::string,
                             std::vector<std::shared_ptr<Subscriber>>> &actual,
    std::ostringstream &oss) {
  for (const auto &[key, expected_subscribers] : expected) {
    auto actual_it = actual.find(key);
    if (actual_it == actual.end()) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "Missing key in actual: " << key << "\n";
      continue;
    }
    const auto &actual_subscribers = actual_it->second;
    if (expected_subscribers.size() != actual_subscribers.size()) {
      oss << conmat::Indent(1) << conmat::TestFailed() << "Key: " << key
          << " subscribers size mismatch:"
          << "\n";
      oss << conmat::Indent(2) << "actual: "
          << conmat::Colorize(actual_subscribers.size(), conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(expected_subscribers.size(), conmat::Color::Blue)
          << "\n";
    }

    // compare each subscriber

    for (size_t i = 0; i < expected_subscribers.size(); ++i) {

      const auto &expected_subscriber = expected_subscribers[i];
      const auto &actual_subscriber = actual_subscribers[i];

      SubscriberEqualsMatcher subscriber_matcher(*expected_subscriber);

      if (!subscriber_matcher.match(*actual_subscriber)) {
        oss << conmat::Indent(1) << conmat::TestFailed() << "Key: " << key
            << " Subscriber index: " << i << " mismatch:"
            << "\n";
        oss << subscriber_matcher.describe();
      }
    }
  }
}
} // namespace steamrot::tests
