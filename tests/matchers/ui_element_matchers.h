/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher for UIElement comparison with recursive support
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "ButtonElement.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "PanelElement.h"
#include "UIElement.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <cmath>
#include <sstream>
#include <string>
#include <typeinfo>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class UIElementEqualsMatcher
/// @brief Custom Catch2 matcher for comparing UIElement objects recursively
///
/// Provides detailed output when UIElements don't match, including
/// child element comparison
/////////////////////////////////////////////////
class UIElementEqualsMatcher : public Catch::Matchers::MatcherBase<UIElement> {
private:
  const UIElement &m_expected;
  mutable std::string m_mismatch_description;

  /////////////////////////////////////////////////
  /// @brief Compare two sf::Vector2f values with epsilon tolerance
  ///
  /// @param actual The actual vector
  /// @param expected The expected vector
  /// @param epsilon The tolerance for floating-point comparison
  /// @return true if vectors are equal within tolerance
  /////////////////////////////////////////////////
  bool VectorsEqual(const sf::Vector2f &actual, const sf::Vector2f &expected,
                    float epsilon = 0.0001f) const {
    return std::abs(actual.x - expected.x) < epsilon &&
           std::abs(actual.y - expected.y) < epsilon;
  }

  /////////////////////////////////////////////////
  /// @brief Compare subscription pointers
  ///
  /// @param actual The actual subscription
  /// @param expected The expected subscription
  /// @return true if subscriptions match
  /////////////////////////////////////////////////
  bool SubscriptionsEqual(const std::shared_ptr<Subscriber> &actual,
                          const std::shared_ptr<Subscriber> &expected) const {
    // Both null
    if (!actual && !expected)
      return true;
    // One null, one not
    if (!actual || !expected)
      return false;
    // Both non-null, compare IsActive state
    return actual->IsActive() == expected->IsActive();
  }

  /////////////////////////////////////////////////
  /// @brief Compare response_event optionals
  ///
  /// @param actual The actual response_event
  /// @param expected The expected response_event
  /// @return true if response_events match
  /////////////////////////////////////////////////
  bool ResponseEventsEqual(const std::optional<EventPacket> &actual,
                           const std::optional<EventPacket> &expected) const {
    // Both empty
    if (!actual.has_value() && !expected.has_value())
      return true;
    // One empty, one not
    if (actual.has_value() != expected.has_value())
      return false;

    return true;
  }

  /////////////////////////////////////////////////
  /// @brief Compare derived type-specific fields
  ///
  /// @param actual The actual UIElement
  /// @param expected The expected UIElement
  /// @param oss Output string stream for mismatch description
  /// @return true if derived fields match
  /////////////////////////////////////////////////
  bool CompareDerivedFields(const UIElement &actual, const UIElement &expected,
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
  /// @brief Recursively compare child elements
  ///
  /// @param actual The actual UIElement
  /// @param expected The expected UIElement
  /// @param oss Output string stream for mismatch description
  /// @param depth Current recursion depth for readable output
  /// @return true if all children match
  /////////////////////////////////////////////////
  bool CompareChildren(const UIElement &actual, const UIElement &expected,
                       std::ostringstream &oss, int depth = 0) const {
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
  /// @brief Compare all fields of two UIElements
  ///
  /// @param actual The actual UIElement
  /// @param expected The expected UIElement
  /// @param oss Output string stream for mismatch description
  /// @param depth Current recursion depth for readable output
  /// @return true if all fields match
  /////////////////////////////////////////////////
  bool CompareUIElements(const UIElement &actual, const UIElement &expected,
                         std::ostringstream &oss, int depth = 0) const {
    std::string prefix =
        depth > 0 ? "child[depth=" + std::to_string(depth) + "]." : "";

    // Compare position
    if (!VectorsEqual(actual.position, expected.position)) {
      oss << prefix << "position: actual=(" << actual.position.x << ","
          << actual.position.y << "), expected=(" << expected.position.x << ","
          << expected.position.y << "); ";
      return false;
    }

    // Compare size
    if (!VectorsEqual(actual.size, expected.size)) {
      oss << prefix << "size: actual=(" << actual.size.x << "," << actual.size.y
          << "), expected=(" << expected.size.x << "," << expected.size.y
          << "); ";
      return false;
    }

    // Compare is_mouse_over
    if (actual.is_mouse_over != expected.is_mouse_over) {
      oss << prefix << "is_mouse_over: actual=" << actual.is_mouse_over
          << ", expected=" << expected.is_mouse_over << "; ";
      return false;
    }

    // Compare children_active
    if (actual.children_active != expected.children_active) {
      oss << prefix << "children_active: actual=" << actual.children_active
          << ", expected=" << expected.children_active << "; ";
      return false;
    }

    // Compare spacing_strategy
    if (actual.spacing_strategy != expected.spacing_strategy) {
      oss << prefix << "spacing_strategy: actual="
          << static_cast<int>(actual.spacing_strategy)
          << ", expected=" << static_cast<int>(expected.spacing_strategy)
          << "; ";
      return false;
    }

    // Compare layout
    if (actual.layout != expected.layout) {
      oss << prefix << "layout: actual=" << static_cast<int>(actual.layout)
          << ", expected=" << static_cast<int>(expected.layout) << "; ";
      return false;
    }

    // Compare subscription
    if (!SubscriptionsEqual(actual.subscription, expected.subscription)) {
      oss << prefix << "subscription differs; ";
      return false;
    }

    // Compare response_event
    if (!ResponseEventsEqual(actual.response_event, expected.response_event)) {
      oss << prefix << "response_event differs; ";
      return false;
    }

    // Compare derived type fields
    if (!CompareDerivedFields(actual, expected, oss)) {
      return false;
    }

    // Recursively compare children
    if (!CompareChildren(actual, expected, oss, depth)) {
      return false;
    }

    return true;
  }

public:
  explicit UIElementEqualsMatcher(const UIElement &expected)
      : m_expected(expected) {}

  ////////////////////////////////////////////////////////////
  /// @brief Check if the UIElement matches
  ///
  /// @param actual The actual UIElement to compare
  /// @return true if UIElements match, false otherwise
  ////////////////////////////////////////////////////////////
  bool match(const UIElement &actual) const override {
    m_mismatch_description.clear();
    std::ostringstream oss;

    bool matches = CompareUIElements(actual, m_expected, oss, 0);

    m_mismatch_description = oss.str();
    return matches;
  }

  ////////////////////////////////////////////////////////////
  /// @brief Describe the matcher
  ///
  /// @return Description string
  ////////////////////////////////////////////////////////////
  std::string describe() const override {
    if (m_mismatch_description.empty()) {
      return "equals UIElement";
    }
    return "UIElement mismatch: " + m_mismatch_description;
  }
};

/////////////////////////////////////////////////
/// @brief Helper function to create UIElementEqualsMatcher
///
/// @param expected The expected UIElement
/// @return UIElementEqualsMatcher instance
/////////////////////////////////////////////////
inline UIElementEqualsMatcher EqualsUIElement(const UIElement &expected) {
  return UIElementEqualsMatcher(expected);
}

} // namespace steamrot::tests
