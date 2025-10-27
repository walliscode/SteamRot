/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for Component custom matchers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ButtonElement.h"
#include "CMeta.h"
#include "CUserInterface.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "PanelElement.h"
#include "cmeta_matchers.h"
#include "component_matchers.h"
#include "cuser_interface_matchers.h"
#include "ui_element_matchers.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

TEST_CASE("CMeta custom matcher works correctly",
          "[unit][Components][CMeta][matcher]") {
  steamrot::CMeta expected;
  expected.m_active = true;
  expected.m_entity_active = true;

  steamrot::CMeta actual;

  SECTION("Matcher detects differences") {
    actual.m_active = false;
    actual.m_entity_active = false;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCMeta(expected));
  }

  SECTION("Matcher detects equality") {
    actual.m_active = true;
    actual.m_entity_active = true;
    REQUIRE_THAT(actual, steamrot::tests::EqualsCMeta(expected));
  }
}

TEST_CASE("CUserInterface custom matcher works correctly",
          "[unit][Components][CUserInterface][matcher]") {
  steamrot::CUserInterface expected;
  expected.m_name = "TestUI";
  expected.m_UI_visible = true;

  steamrot::CUserInterface actual;

  SECTION("Matcher detects differences in m_name") {
    actual.m_name = "DifferentUI";
    actual.m_UI_visible = true;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUserInterface(expected));
  }

  SECTION("Matcher detects equality") {
    actual.m_name = "TestUI";
    actual.m_UI_visible = true;
    REQUIRE_THAT(actual, steamrot::tests::EqualsCUserInterface(expected));
  }

  SECTION("Matcher compares root elements when both are null") {
    expected.m_root_element = nullptr;
    actual.m_root_element = nullptr;
    actual.m_name = "TestUI";
    actual.m_UI_visible = true;
    REQUIRE_THAT(actual, steamrot::tests::EqualsCUserInterface(expected));
  }

  SECTION("Matcher detects when one root element is null") {
    expected.m_root_element = std::make_unique<steamrot::PanelElement>();
    actual.m_root_element = nullptr;
    actual.m_name = "TestUI";
    actual.m_UI_visible = true;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUserInterface(expected));
  }

  SECTION("Matcher compares root elements recursively") {
    auto expected_panel = std::make_unique<steamrot::PanelElement>();
    expected_panel->position = sf::Vector2f{100.f, 200.f};
    expected_panel->size = sf::Vector2f{50.f, 30.f};
    expected.m_root_element = std::move(expected_panel);

    auto actual_panel = std::make_unique<steamrot::PanelElement>();
    actual_panel->position = sf::Vector2f{100.f, 200.f};
    actual_panel->size = sf::Vector2f{50.f, 30.f};
    actual.m_root_element = std::move(actual_panel);

    actual.m_name = "TestUI";
    actual.m_UI_visible = true;
    REQUIRE_THAT(actual, steamrot::tests::EqualsCUserInterface(expected));
  }

  SECTION("Matcher detects differences in root element fields") {
    auto expected_panel = std::make_unique<steamrot::PanelElement>();
    expected_panel->position = sf::Vector2f{100.f, 200.f};
    expected.m_root_element = std::move(expected_panel);

    auto actual_panel = std::make_unique<steamrot::PanelElement>();
    actual_panel->position = sf::Vector2f{150.f, 200.f}; // Different position
    actual.m_root_element = std::move(actual_panel);

    actual.m_name = "TestUI";
    actual.m_UI_visible = true;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUserInterface(expected));
  }
}

TEST_CASE("Generic Component matcher works correctly",
          "[unit][Components][matcher]") {
  SECTION("Works with CMeta") {
    steamrot::CMeta expected;
    expected.m_active = true;

    steamrot::CMeta actual;
    actual.m_active = true;

    REQUIRE_THAT(actual, steamrot::tests::EqualsComponent(expected));
  }

  SECTION("Detects differences with CMeta") {
    steamrot::CMeta expected;
    expected.m_active = true;

    steamrot::CMeta actual;
    actual.m_active = false;

    REQUIRE_THAT(actual, !steamrot::tests::EqualsComponent(expected));
  }
}

TEST_CASE("UIElement matcher compares base fields correctly",
          "[unit][UIElement][matcher]") {
  SECTION("Matcher detects equality for PanelElement") {
    steamrot::PanelElement expected;
    expected.position = sf::Vector2f{100.f, 200.f};
    expected.size = sf::Vector2f{50.f, 30.f};
    expected.is_mouse_over = false;
    expected.children_active = true;

    steamrot::PanelElement actual;
    actual.position = sf::Vector2f{100.f, 200.f};
    actual.size = sf::Vector2f{50.f, 30.f};
    actual.is_mouse_over = false;
    actual.children_active = true;

    REQUIRE_THAT(actual, steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects position differences") {
    steamrot::PanelElement expected;
    expected.position = sf::Vector2f{100.f, 200.f};

    steamrot::PanelElement actual;
    actual.position = sf::Vector2f{150.f, 200.f};

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects size differences") {
    steamrot::PanelElement expected;
    expected.size = sf::Vector2f{50.f, 30.f};

    steamrot::PanelElement actual;
    actual.size = sf::Vector2f{50.f, 40.f};

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects is_mouse_over differences") {
    steamrot::PanelElement expected;
    expected.is_mouse_over = true;

    steamrot::PanelElement actual;
    actual.is_mouse_over = false;

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects children_active differences") {
    steamrot::PanelElement expected;
    expected.children_active = true;

    steamrot::PanelElement actual;
    actual.children_active = false;

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects layout differences") {
    steamrot::PanelElement expected;
    expected.layout = steamrot::LayoutType::LayoutType_Horizontal;

    steamrot::PanelElement actual;
    actual.layout = steamrot::LayoutType::LayoutType_Vertical;

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects spacing_strategy differences") {
    steamrot::PanelElement expected;
    expected.spacing_strategy =
        steamrot::SpacingAndSizingType::SpacingAndSizingType_Even;

    steamrot::PanelElement actual;
    actual.spacing_strategy =
        steamrot::SpacingAndSizingType::SpacingAndSizingType_None;

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }
}

TEST_CASE("UIElement matcher compares derived types correctly",
          "[unit][UIElement][matcher]") {
  SECTION("Matcher compares ButtonElement label") {
    steamrot::ButtonElement expected;
    expected.label = "Click Me";

    steamrot::ButtonElement actual;
    actual.label = "Click Me";

    REQUIRE_THAT(actual, steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects ButtonElement label differences") {
    steamrot::ButtonElement expected;
    expected.label = "Click Me";

    steamrot::ButtonElement actual;
    actual.label = "Different Label";

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher compares DropDownButtonElement is_expanded") {
    steamrot::DropDownButtonElement expected;
    expected.is_expanded = true;

    steamrot::DropDownButtonElement actual;
    actual.is_expanded = true;

    REQUIRE_THAT(actual, steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects DropDownButtonElement is_expanded differences") {
    steamrot::DropDownButtonElement expected;
    expected.is_expanded = true;

    steamrot::DropDownButtonElement actual;
    actual.is_expanded = false;

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher compares DropDownListElement fields") {
    steamrot::DropDownListElement expected;
    expected.is_expanded = true;
    expected.unexpanded_label = "Select...";
    expected.expanded_label = "Items";

    steamrot::DropDownListElement actual;
    actual.is_expanded = true;
    actual.unexpanded_label = "Select...";
    actual.expanded_label = "Items";

    REQUIRE_THAT(actual, steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects DropDownListElement unexpanded_label differences") {
    steamrot::DropDownListElement expected;
    expected.unexpanded_label = "Select...";

    steamrot::DropDownListElement actual;
    actual.unexpanded_label = "Choose...";

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher compares DropDownItemElement fields") {
    steamrot::DropDownItemElement expected;
    expected.label = "Item 1";
    expected.value = "value1";

    steamrot::DropDownItemElement actual;
    actual.label = "Item 1";
    actual.value = "value1";

    REQUIRE_THAT(actual, steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects DropDownItemElement value differences") {
    steamrot::DropDownItemElement expected;
    expected.value = "value1";

    steamrot::DropDownItemElement actual;
    actual.value = "value2";

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects type mismatches") {
    steamrot::ButtonElement expected;
    steamrot::PanelElement actual;

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }
}

TEST_CASE("UIElement matcher handles child elements recursively",
          "[unit][UIElement][matcher]") {
  SECTION("Matcher detects equality with no children") {
    steamrot::PanelElement expected;
    steamrot::PanelElement actual;

    REQUIRE_THAT(actual, steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher compares single child element") {
    steamrot::PanelElement expected;
    auto expected_child = std::make_unique<steamrot::ButtonElement>();
    expected_child->label = "Child Button";
    expected.child_elements.push_back(std::move(expected_child));

    steamrot::PanelElement actual;
    auto actual_child = std::make_unique<steamrot::ButtonElement>();
    actual_child->label = "Child Button";
    actual.child_elements.push_back(std::move(actual_child));

    REQUIRE_THAT(actual, steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects child element differences") {
    steamrot::PanelElement expected;
    auto expected_child = std::make_unique<steamrot::ButtonElement>();
    expected_child->label = "Child Button";
    expected.child_elements.push_back(std::move(expected_child));

    steamrot::PanelElement actual;
    auto actual_child = std::make_unique<steamrot::ButtonElement>();
    actual_child->label = "Different Label";
    actual.child_elements.push_back(std::move(actual_child));

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects child count differences") {
    steamrot::PanelElement expected;
    expected.child_elements.push_back(
        std::make_unique<steamrot::ButtonElement>());

    steamrot::PanelElement actual;
    actual.child_elements.push_back(
        std::make_unique<steamrot::ButtonElement>());
    actual.child_elements.push_back(
        std::make_unique<steamrot::ButtonElement>());

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher compares nested children (depth > 1)") {
    steamrot::PanelElement expected;
    auto expected_child = std::make_unique<steamrot::PanelElement>();
    expected_child->position = sf::Vector2f{10.f, 10.f};
    auto expected_grandchild = std::make_unique<steamrot::ButtonElement>();
    expected_grandchild->label = "Grandchild";
    expected_child->child_elements.push_back(std::move(expected_grandchild));
    expected.child_elements.push_back(std::move(expected_child));

    steamrot::PanelElement actual;
    auto actual_child = std::make_unique<steamrot::PanelElement>();
    actual_child->position = sf::Vector2f{10.f, 10.f};
    auto actual_grandchild = std::make_unique<steamrot::ButtonElement>();
    actual_grandchild->label = "Grandchild";
    actual_child->child_elements.push_back(std::move(actual_grandchild));
    actual.child_elements.push_back(std::move(actual_child));

    REQUIRE_THAT(actual, steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects nested children differences") {
    steamrot::PanelElement expected;
    auto expected_child = std::make_unique<steamrot::PanelElement>();
    auto expected_grandchild = std::make_unique<steamrot::ButtonElement>();
    expected_grandchild->label = "Grandchild";
    expected_child->child_elements.push_back(std::move(expected_grandchild));
    expected.child_elements.push_back(std::move(expected_child));

    steamrot::PanelElement actual;
    auto actual_child = std::make_unique<steamrot::PanelElement>();
    auto actual_grandchild = std::make_unique<steamrot::ButtonElement>();
    actual_grandchild->label = "Different Grandchild";
    actual_child->child_elements.push_back(std::move(actual_grandchild));
    actual.child_elements.push_back(std::move(actual_child));

    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher handles multiple children at same level") {
    steamrot::PanelElement expected;
    auto expected_child1 = std::make_unique<steamrot::ButtonElement>();
    expected_child1->label = "Button 1";
    auto expected_child2 = std::make_unique<steamrot::ButtonElement>();
    expected_child2->label = "Button 2";
    expected.child_elements.push_back(std::move(expected_child1));
    expected.child_elements.push_back(std::move(expected_child2));

    steamrot::PanelElement actual;
    auto actual_child1 = std::make_unique<steamrot::ButtonElement>();
    actual_child1->label = "Button 1";
    auto actual_child2 = std::make_unique<steamrot::ButtonElement>();
    actual_child2->label = "Button 2";
    actual.child_elements.push_back(std::move(actual_child1));
    actual.child_elements.push_back(std::move(actual_child2));

    REQUIRE_THAT(actual, steamrot::tests::EqualsUIElement(expected));
  }
}
