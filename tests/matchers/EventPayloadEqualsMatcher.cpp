/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for EventPayload comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPayloadEqualsMatcher.h"
#include "conmat.h"

namespace steamrot::tests {
/////////////////////////////////////////////////
EventPayloadEqualsMatcher::EventPayloadEqualsMatcher(
    const EventPayload &expected)
    : m_expected(expected) {}

/////////////////////////////////////////////////
std::string EventPayloadEqualsMatcher::GetNameForEventPayloadIndex(
    size_t index) const {
  switch (index) {
  case 0:
    return "InputPayload";
  case 1:
    return "UIPayload";
  case 2:
    return "LogicPayload";
  case 3:
    return "ScenePayload";
  case 4:
    return "SystemPayload";
  default:
    return "Unknown";
  }
}

/////////////////////////////////////////////////
bool EventPayloadEqualsMatcher::match(const EventPayload &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  // Check if variant types match and return early if they don't
  if (actual.index() != m_expected.index()) {
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "EventPayload variant type differs:" << "\n";
    oss << conmat::Indent(2) << "actual: "
        << conmat::Colorize(GetNameForEventPayloadIndex(actual.index()),
                            conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected: "
        << conmat::Colorize(GetNameForEventPayloadIndex(m_expected.index()),
                            conmat::Color::Blue)
        << "\n";
    m_mismatch_description = oss.str();
    return false;
  }

  // Compare based on variant type
  if (std::holds_alternative<InputPayload>(actual)) {
    const auto &actual_payload = std::get<InputPayload>(actual);
    const auto &expected_payload = std::get<InputPayload>(m_expected);

    if (actual_payload.action != expected_payload.action) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "InputPayload action differs:" << "\n";
      oss << conmat::Indent(2) << "actual: "
          << conmat::Colorize(
                 static_cast<int>(actual_payload.action), conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(static_cast<int>(expected_payload.action),
                              conmat::Color::Blue)
          << "\n";
    }

  } else if (std::holds_alternative<UIPayload>(actual)) {
    const auto &actual_payload = std::get<UIPayload>(actual);
    const auto &expected_payload = std::get<UIPayload>(m_expected);

    // Compare c_user_interface_name
    if (actual_payload.c_user_interface_name.has_value() !=
        expected_payload.c_user_interface_name.has_value()) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "UIPayload c_user_interface_name presence differs:" << "\n";
      oss << conmat::Indent(2) << "actual has value: "
          << conmat::Colorize(
                 actual_payload.c_user_interface_name.has_value(),
                 conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected has value: "
          << conmat::Colorize(
                 expected_payload.c_user_interface_name.has_value(),
                 conmat::Color::Blue)
          << "\n";
    } else if (actual_payload.c_user_interface_name.has_value() &&
               actual_payload.c_user_interface_name.value() !=
                   expected_payload.c_user_interface_name.value()) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "UIPayload c_user_interface_name differs:" << "\n";
      oss << conmat::Indent(2) << "actual: "
          << conmat::Colorize(actual_payload.c_user_interface_name.value(),
                              conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(expected_payload.c_user_interface_name.value(),
                              conmat::Color::Blue)
          << "\n";
    }

    // Compare c_ui_state_name
    if (actual_payload.c_ui_state_name.has_value() !=
        expected_payload.c_ui_state_name.has_value()) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "UIPayload c_ui_state_name presence differs:" << "\n";
      oss << conmat::Indent(2) << "actual has value: "
          << conmat::Colorize(actual_payload.c_ui_state_name.has_value(),
                              conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected has value: "
          << conmat::Colorize(expected_payload.c_ui_state_name.has_value(),
                              conmat::Color::Blue)
          << "\n";
    } else if (actual_payload.c_ui_state_name.has_value() &&
               actual_payload.c_ui_state_name.value() !=
                   expected_payload.c_ui_state_name.value()) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "UIPayload c_ui_state_name differs:" << "\n";
      oss << conmat::Indent(2) << "actual: "
          << conmat::Colorize(actual_payload.c_ui_state_name.value(),
                              conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(expected_payload.c_ui_state_name.value(),
                              conmat::Color::Blue)
          << "\n";
    }

    // Compare action
    if (actual_payload.action != expected_payload.action) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "UIPayload action differs:" << "\n";
      oss << conmat::Indent(2) << "actual: "
          << conmat::Colorize(
                 static_cast<int>(actual_payload.action), conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(static_cast<int>(expected_payload.action),
                              conmat::Color::Blue)
          << "\n";
    }

  } else if (std::holds_alternative<LogicPayload>(actual)) {
    const auto &actual_payload = std::get<LogicPayload>(actual);
    const auto &expected_payload = std::get<LogicPayload>(m_expected);

    if (actual_payload.toggle_name != expected_payload.toggle_name) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "LogicPayload toggle_name differs:" << "\n";
      oss << conmat::Indent(2) << "actual: "
          << conmat::Colorize(static_cast<int>(actual_payload.toggle_name),
                              conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(static_cast<int>(expected_payload.toggle_name),
                              conmat::Color::Blue)
          << "\n";
    }

  } else if (std::holds_alternative<ScenePayload>(actual)) {
    const auto &actual_payload = std::get<ScenePayload>(actual);
    const auto &expected_payload = std::get<ScenePayload>(m_expected);

    // Compare scene_type
    if (actual_payload.scene_type.has_value() !=
        expected_payload.scene_type.has_value()) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "ScenePayload scene_type presence differs:" << "\n";
      oss << conmat::Indent(2) << "actual has value: "
          << conmat::Colorize(actual_payload.scene_type.has_value(),
                              conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected has value: "
          << conmat::Colorize(expected_payload.scene_type.has_value(),
                              conmat::Color::Blue)
          << "\n";
    } else if (actual_payload.scene_type.has_value() &&
               actual_payload.scene_type.value() !=
                   expected_payload.scene_type.value()) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "ScenePayload scene_type differs:" << "\n";
      oss << conmat::Indent(2) << "actual: "
          << conmat::Colorize(
                 EnumNameSceneType(actual_payload.scene_type.value()),
                 conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(
                 EnumNameSceneType(expected_payload.scene_type.value()),
                 conmat::Color::Blue)
          << "\n";
    }

    // Compare scene_id
    if (actual_payload.scene_id.has_value() !=
        expected_payload.scene_id.has_value()) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "ScenePayload scene_id presence differs:" << "\n";
      oss << conmat::Indent(2) << "actual has value: "
          << conmat::Colorize(actual_payload.scene_id.has_value(),
                              conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected has value: "
          << conmat::Colorize(expected_payload.scene_id.has_value(),
                              conmat::Color::Blue)
          << "\n";
    } else if (actual_payload.scene_id.has_value() &&
               actual_payload.scene_id.value() !=
                   expected_payload.scene_id.value()) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "ScenePayload scene_id differs:" << "\n";
      oss << conmat::Indent(2) << "actual: "
          << conmat::Colorize(actual_payload.scene_id.value(),
                              conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(expected_payload.scene_id.value(),
                              conmat::Color::Blue)
          << "\n";
    }

    // Compare action
    if (actual_payload.action != expected_payload.action) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "ScenePayload action differs:" << "\n";
      oss << conmat::Indent(2) << "actual: "
          << conmat::Colorize(
                 static_cast<int>(actual_payload.action), conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(static_cast<int>(expected_payload.action),
                              conmat::Color::Blue)
          << "\n";
    }

  } else if (std::holds_alternative<SystemPayload>(actual)) {
    const auto &actual_payload = std::get<SystemPayload>(actual);
    const auto &expected_payload = std::get<SystemPayload>(m_expected);

    if (actual_payload.action != expected_payload.action) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "SystemPayload action differs:" << "\n";
      oss << conmat::Indent(2) << "actual: "
          << conmat::Colorize(
                 static_cast<int>(actual_payload.action), conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected: "
          << conmat::Colorize(static_cast<int>(expected_payload.action),
                              conmat::Color::Blue)
          << "\n";
    }
  }

  m_mismatch_description = oss.str();
  return m_mismatch_description.empty();
}

/////////////////////////////////////////////////
std::string EventPayloadEqualsMatcher::describe() const {
  if (m_mismatch_description.empty()) {
    return conmat::Header(conmat::TestPassed() + "EventPayload Match:", 3) +
           "\n";
  } else {
    return conmat::Header(conmat::TestFailed() + "EventPayload Mismatch:", 3) +
           "\n" + m_mismatch_description;
  }
}

} // namespace steamrot::tests
