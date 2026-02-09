/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for UIElement comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIElementEqualsMatcher.h"
#include "ButtonElement.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "conmat.h"
#include <cmath>
#include <typeinfo>

namespace steamrot::tests {

/////////////////////////////////////////////////
bool UIElementEqualsMatcher::VectorsEqual(const sf::Vector2f &actual,
                                          const sf::Vector2f &expected,
                                          float epsilon) const {
  return std::abs(actual.x - expected.x) < epsilon &&
         std::abs(actual.y - expected.y) < epsilon;
}

/////////////////////////////////////////////////
bool UIElementEqualsMatcher::SubscriptionsEqual(
    const std::shared_ptr<Subscriber> &actual,
    const std::shared_ptr<Subscriber> &expected) const {
  // Both null
  if (!actual && !expected)
    return true;
  // One null, one not
  if (!actual || !expected)
    return false;
  // Both non-null, compare m_active state
  return actual->m_active == expected->m_active;
}

/////////////////////////////////////////////////
bool UIElementEqualsMatcher::ResponseEventsEqual(
    const std::vector<EventPacket> &actual,
    const std::vector<EventPacket> &expected) const {
  // Compare sizes first
  if (actual.size() != expected.size())
    return false;

  // For now, just check that sizes match
  // Could add more detailed comparison if needed
  return true;
}

/////////////////////////////////////////////////
bool UIElementEqualsMatcher::CompareDerivedFields(
    const UIElement &actual, const UIElement &expected,
    std::ostringstream &oss) const {
  // Check if types match
  if (typeid(actual) != typeid(expected)) {
    oss << "UIElement types differ: actual=" << typeid(actual).name()
        << ", expected=" << typeid(expected).name() << "; ";
    return false;
  }

  // Compare ButtonElement fields
  if (auto actual_btn = dynamic_cast<const ButtonElement *>(&actual)) {
    auto expected_btn = static_cast<const ButtonElement *>(&expected);
    if (actual_btn->label != expected_btn->label) {
      oss << "ButtonElement.label: actual='" << actual_btn->label
          << "', expected='" << expected_btn->label << "'; ";
      return false;
    }
  }
  // Compare DropDownButtonElement fields
  else if (auto actual_dd_btn =
               dynamic_cast<const DropDownButtonElement *>(&actual)) {
    auto expected_dd_btn =
        static_cast<const DropDownButtonElement *>(&expected);
    if (actual_dd_btn->is_expanded != expected_dd_btn->is_expanded) {
      oss << "DropDownButtonElement.is_expanded: actual="
          << actual_dd_btn->is_expanded
          << ", expected=" << expected_dd_btn->is_expanded << "; ";
      return false;
    }
  }
  // Compare DropDownContainerElement fields
  else if (auto actual_dd_cont =
               dynamic_cast<const DropDownContainerElement *>(&actual)) {
    auto expected_dd_cont =
        static_cast<const DropDownContainerElement *>(&expected);
    if (actual_dd_cont->is_expanded != expected_dd_cont->is_expanded) {
      oss << "DropDownContainerElement.is_expanded: actual="
          << actual_dd_cont->is_expanded
          << ", expected=" << expected_dd_cont->is_expanded << "; ";
      return false;
    }
  }
  // Compare DropDownListElement fields
  else if (auto actual_dd_list =
               dynamic_cast<const DropDownListElement *>(&actual)) {
    auto expected_dd_list =
        static_cast<const DropDownListElement *>(&expected);
    if (actual_dd_list->is_expanded != expected_dd_list->is_expanded) {
      oss << "DropDownListElement.is_expanded: actual="
          << actual_dd_list->is_expanded
          << ", expected=" << expected_dd_list->is_expanded << "; ";
      return false;
    }
    if (actual_dd_list->unexpanded_label !=
        expected_dd_list->unexpanded_label) {
      oss << "DropDownListElement.unexpanded_label: actual='"
          << actual_dd_list->unexpanded_label << "', expected='"
          << expected_dd_list->unexpanded_label << "'; ";
      return false;
    }
    if (actual_dd_list->expanded_label != expected_dd_list->expanded_label) {
      oss << "DropDownListElement.expanded_label: actual='"
          << actual_dd_list->expanded_label << "', expected='"
          << expected_dd_list->expanded_label << "'; ";
      return false;
    }
    if (actual_dd_list->data_populate_function !=
        expected_dd_list->data_populate_function) {
      oss << "DropDownListElement.data_populate_function: actual="
          << static_cast<int>(actual_dd_list->data_populate_function)
          << ", expected="
          << static_cast<int>(expected_dd_list->data_populate_function)
          << "; ";
      return false;
    }
  }
  // Compare DropDownItemElement fields
  else if (auto actual_dd_item =
               dynamic_cast<const DropDownItemElement *>(&actual)) {
    auto expected_dd_item =
        static_cast<const DropDownItemElement *>(&expected);
    if (actual_dd_item->label != expected_dd_item->label) {
      oss << "DropDownItemElement.label: actual='" << actual_dd_item->label
          << "', expected='" << expected_dd_item->label << "'; ";
      return false;
    }
    if (actual_dd_item->value != expected_dd_item->value) {
      oss << "DropDownItemElement.value: actual='" << actual_dd_item->value
          << "', expected='" << expected_dd_item->value << "'; ";
      return false;
    }
  }
  // PanelElement has no extra fields beyond UIElement

  return true;
}

/////////////////////////////////////////////////
bool UIElementEqualsMatcher::CompareChildren(const UIElement &actual,
                                             const UIElement &expected,
                                             std::ostringstream &oss,
                                             int depth) const {
  if (actual.child_elements.size() != expected.child_elements.size()) {
    oss << "child_elements.size() at depth " << depth
        << ": actual=" << actual.child_elements.size()
        << ", expected=" << expected.child_elements.size() << "; ";
    return false;
  }

  for (size_t i = 0; i < actual.child_elements.size(); ++i) {
    const auto &actual_child = actual.child_elements[i];
    const auto &expected_child = expected.child_elements[i];

    if (!actual_child && !expected_child)
      continue;

    if (!actual_child || !expected_child) {
      oss << "child_elements[" << i << "] at depth " << depth
          << ": one is null, other is not; ";
      return false;
    }

    // Recursively compare child element
    if (!CompareUIElements(*actual_child, *expected_child, oss, depth + 1)) {
      return false;
    }
  }

  return true;
}

/////////////////////////////////////////////////
bool UIElementEqualsMatcher::CompareUIElements(
    const UIElement &actual, const UIElement &expected,
    std::ostringstream &output_oss, int depth) const {

  std::ostringstream oss;
  std::string prefix =
      depth > 0 ? "child[depth=" + std::to_string(depth) + "]." : "";

  // Compare position
  if (!VectorsEqual(actual.position, expected.position)) {
    oss << prefix << "position:" << "\n";
    oss << "actual = ("
        << conmat::Colorize(actual.position.x, conmat::Color::Red) << ","
        << conmat::Colorize(actual.position.y, conmat::Color::Red) << ")"
        << "\n";
    oss << "expected = ("
        << conmat::Colorize(expected.position.x, conmat::Color::Blue) << ","
        << conmat::Colorize(expected.position.y, conmat::Color::Blue) << ")"
        << "\n";
  }

  // Compare size
  if (!VectorsEqual(actual.size, expected.size)) {
    oss << prefix << "size: actual=(" << actual.size.x << "," << actual.size.y
        << "), expected=(" << expected.size.x << "," << expected.size.y
        << "); ";
  }

  // Compare is_mouse_over
  if (actual.is_mouse_over != expected.is_mouse_over) {
    oss << prefix << "is_mouse_over: actual=" << actual.is_mouse_over
        << ", expected=" << expected.is_mouse_over << "; ";
  }

  if (actual.children_active != expected.children_active) {
    oss << prefix << "children_active: actual=" << actual.children_active
        << ", expected=" << expected.children_active << "; ";
  }

  // Compare spacing_strategy
  if (actual.spacing_strategy != expected.spacing_strategy) {
    oss << prefix << "spacing_strategy: actual="
        << static_cast<int>(actual.spacing_strategy)
        << ", expected=" << static_cast<int>(expected.spacing_strategy)
        << "; ";
  }

  // Compare layout
  if (actual.layout != expected.layout) {
    oss << prefix << "layout: actual=" << static_cast<int>(actual.layout)
        << ", expected=" << static_cast<int>(expected.layout) << "; ";
  }

  // Compare subscription
  if (!SubscriptionsEqual(actual.subscription, expected.subscription)) {
    oss << prefix << "subscription differs; ";
  }

  // Compare response_events
  if (!ResponseEventsEqual(actual.response_events, expected.response_events)) {
    oss << prefix << "response_events differs; ";
  }

  // Compare derived type fields
  if (!CompareDerivedFields(actual, expected, oss)) {
  }

  // Recursively compare children
  if (!CompareChildren(actual, expected, oss, depth)) {
  }

  if (oss.str().empty()) {
    return true;
  } else {
    output_oss << oss.str();
    return false;
  }
}

/////////////////////////////////////////////////
UIElementEqualsMatcher::UIElementEqualsMatcher(const UIElement &expected)
    : m_expected(expected) {}

/////////////////////////////////////////////////
bool UIElementEqualsMatcher::match(const UIElement &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  bool matches = CompareUIElements(actual, m_expected, oss, 0);

  m_mismatch_description = oss.str();
  return matches;
}

/////////////////////////////////////////////////
std::string UIElementEqualsMatcher::describe() const {

  // if mismatch description is empty then we can assume the test passed
  if (m_mismatch_description.empty()) {

    std::ostringstream oss;
    oss << conmat::Divider("-", 40) << "\n";
    oss << conmat::TestPassed() << "UIElement Match" << "\n";
    oss << conmat::Divider("-", 40) << "\n";
    return oss.str();
  } else {

    std::ostringstream oss;

    oss << conmat::TestFailed() << "UIElement Match: " << "\n";
    oss << m_mismatch_description << "\n";
    oss << conmat::Divider("-", 40) << "\n";

    return oss.str();
  }
}

} // namespace steamrot::tests
