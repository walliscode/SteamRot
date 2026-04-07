/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for positioning_ui free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_ui.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "Layout.h"
#include "PanelElement.h"
#include "Vector2fEqualsMatcher.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UpdateSizeAndPositionOfChildElements does nothing when element has "
          "no children",
          "[unit][positioning_ui]") {
  // Arrange
  steamrot::PanelElement panel;
  panel.position = {10.0f, 10.0f};
  panel.size = {200.0f, 100.0f};

  steamrot::UIStyle style;

  // Act - should not modify anything, just not crash
  steamrot::logic::positioning::ui::UpdateSizeAndPositionOfChildElements(panel,
                                                                         style);

  // Assert - panel remains unchanged
  REQUIRE_THAT(panel.position, steamrot::tests::EqualsVector2f({10.0f, 10.0f}));
  REQUIRE_THAT(panel.size, steamrot::tests::EqualsVector2f({200.0f, 100.0f}));
}

TEST_CASE("UpdateSizeAndPositionOfChildElements positions Vertical layout "
          "children correctly",
          "[unit][positioning_ui]") {
  // Arrange
  steamrot::PanelElement parent;
  parent.position = {0.0f, 0.0f};
  parent.size = {200.0f, 300.0f};
  parent.layout = steamrot::Layout::Vertical;

  // Add two children
  auto child1 = std::make_unique<steamrot::PanelElement>();
  auto child2 = std::make_unique<steamrot::PanelElement>();
  parent.child_elements.push_back(std::move(child1));
  parent.child_elements.push_back(std::move(child2));

  steamrot::UIStyle style;
  style.panel_style.border_thickness = 5.0f;
  style.panel_style.inner_margin = {10.0f, 10.0f};

  // Act
  steamrot::logic::positioning::ui::UpdateSizeAndPositionOfChildElements(parent,
                                                                         style);

  // Assert
  // available_width = 200 - 2*5 - 2*10 = 170
  // available_height = 300 - 2*5 - 2*10 = 270
  // child_height = (270 - 1*10) / 2 = 130
  // start_position = {0+5+10, 0+5+10} = {15, 15}
  float expected_width = 200.0f - 2 * 5.0f - 2 * 10.0f;
  float expected_height = 300.0f - 2 * 5.0f - 2 * 10.0f;
  float child_height = (expected_height - 1 * 10.0f) / 2.0f;

  REQUIRE_THAT(parent.child_elements[0]->size,
               steamrot::tests::EqualsVector2f({expected_width, child_height}));
  REQUIRE_THAT(parent.child_elements[0]->position,
               steamrot::tests::EqualsVector2f({15.0f, 15.0f}));

  REQUIRE_THAT(parent.child_elements[1]->size,
               steamrot::tests::EqualsVector2f({expected_width, child_height}));
  REQUIRE_THAT(
      parent.child_elements[1]->position,
      steamrot::tests::EqualsVector2f({15.0f, 15.0f + child_height + 10.0f}));
}

TEST_CASE("UpdateSizeAndPositionOfChildElements positions Horizontal layout "
          "children correctly",
          "[unit][positioning_ui]") {
  // Arrange
  steamrot::PanelElement parent;
  parent.position = {0.0f, 0.0f};
  parent.size = {300.0f, 100.0f};
  parent.layout = steamrot::Layout::Horizontal;

  // Add two children
  parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());
  parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());

  steamrot::UIStyle style;
  style.panel_style.border_thickness = 5.0f;
  style.panel_style.inner_margin = {10.0f, 10.0f};

  // Act
  steamrot::logic::positioning::ui::UpdateSizeAndPositionOfChildElements(parent,
                                                                         style);

  // Assert
  // available_width = 300 - 2*5 - 2*10 = 270
  // available_height = 100 - 2*5 - 2*10 = 70
  // child_width = (270 - 1*10) / 2 = 130
  // start_position = {0+5+10, 0+5+10} = {15, 15}
  float available_width = 300.0f - 2 * 5.0f - 2 * 10.0f;
  float available_height = 100.0f - 2 * 5.0f - 2 * 10.0f;
  float child_width = (available_width - 1 * 10.0f) / 2.0f;

  REQUIRE_THAT(
      parent.child_elements[0]->size,
      steamrot::tests::EqualsVector2f({child_width, available_height}));
  REQUIRE_THAT(parent.child_elements[0]->position,
               steamrot::tests::EqualsVector2f({15.0f, 15.0f}));

  REQUIRE_THAT(
      parent.child_elements[1]->size,
      steamrot::tests::EqualsVector2f({child_width, available_height}));
  REQUIRE_THAT(
      parent.child_elements[1]->position,
      steamrot::tests::EqualsVector2f({15.0f + child_width + 10.0f, 15.0f}));
}

TEST_CASE("UpdateSizeAndPositionOfChildElements positions DropDown layout "
          "children correctly",
          "[unit][positioning_ui]") {
  // Arrange
  steamrot::PanelElement parent;
  parent.position = {0.0f, 0.0f};
  parent.size = {200.0f, 50.0f};
  parent.layout = steamrot::Layout::DropDown;

  // Add two children (items stack vertically)
  parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());
  parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());

  steamrot::UIStyle style;
  style.panel_style.border_thickness = 5.0f;
  style.panel_style.inner_margin = {10.0f, 10.0f};

  // Act
  steamrot::logic::positioning::ui::UpdateSizeAndPositionOfChildElements(parent,
                                                                         style);

  // Assert
  // available_width = 200 - 2*5 = 190
  // available_height = 50 - 2*5 = 40
  // start_position = {0+5, 0+5} = {5, 5}
  float available_width = 200.0f - 2 * 5.0f;
  float available_height = 50.0f - 2 * 5.0f;

  REQUIRE_THAT(
      parent.child_elements[0]->size,
      steamrot::tests::EqualsVector2f({available_width, available_height}));
  REQUIRE_THAT(parent.child_elements[0]->position,
               steamrot::tests::EqualsVector2f({5.0f, 5.0f}));

  REQUIRE_THAT(
      parent.child_elements[1]->size,
      steamrot::tests::EqualsVector2f({available_width, available_height}));
  REQUIRE_THAT(
      parent.child_elements[1]->position,
      steamrot::tests::EqualsVector2f({5.0f, 5.0f + available_height}));
}

TEST_CASE("UpdateSizeAndPositionOfChildElements positions "
          "DropDownContainerElement children correctly",
          "[unit][positioning_ui]") {
  // Arrange
  steamrot::DropDownContainerElement dd_container;
  dd_container.position = {0.0f, 0.0f};
  dd_container.size = {200.0f, 50.0f};

  dd_container.child_elements.push_back(
      std::make_unique<steamrot::DropDownListElement>());
  dd_container.child_elements.push_back(
      std::make_unique<steamrot::DropDownButtonElement>());

  steamrot::UIStyle style;
  style.drop_down_container_style.border_thickness = 2.0f;
  style.drop_down_container_style.drop_symbol_ratio = 0.2f;

  // Act
  steamrot::logic::positioning::ui::UpdateSizeAndPositionOfChildElements(
      dd_container, style);

  // Assert
  // available_width = 200 - 2*2 = 196
  // available_height = 50 - 2*2 = 46
  // dd_list_size = {196 * (1 - 0.2), 46} = {156.8, 46}
  // dd_button_size = {196 * 0.2, 46} = {39.2, 46}
  // dd_list_position = {0+2, 0+2} = {2, 2}
  // dd_button_position = {2+156.8, 2} = {158.8, 2}
  float available_width = 200.0f - 2 * 2.0f;
  float available_height = 50.0f - 2 * 2.0f;
  float ratio = 0.2f;
  float dd_list_width = available_width * (1.0f - ratio);
  float dd_button_width = available_width * ratio;

  REQUIRE_THAT(
      dd_container.child_elements[0]->size,
      steamrot::tests::EqualsVector2f({dd_list_width, available_height}));
  REQUIRE_THAT(dd_container.child_elements[0]->position,
               steamrot::tests::EqualsVector2f({2.0f, 2.0f}));

  REQUIRE_THAT(
      dd_container.child_elements[1]->size,
      steamrot::tests::EqualsVector2f({dd_button_width, available_height}));
  REQUIRE_THAT(dd_container.child_elements[1]->position,
               steamrot::tests::EqualsVector2f({2.0f + dd_list_width, 2.0f}));
}

TEST_CASE("PositionNestedUIElements recursively positions active children",
          "[unit][positioning_ui]") {
  // Arrange - parent with one active child that itself has a child
  steamrot::PanelElement root;
  root.position = {0.0f, 0.0f};
  root.size = {200.0f, 200.0f};
  root.layout = steamrot::Layout::Vertical;
  root.children_active = true;

  auto child = std::make_unique<steamrot::PanelElement>();
  child->layout = steamrot::Layout::Horizontal;
  child->children_active = true;

  // grandchild - will be positioned by child's UpdateSizeAndPosition call
  child->child_elements.push_back(std::make_unique<steamrot::PanelElement>());
  child->child_elements.push_back(std::make_unique<steamrot::PanelElement>());

  root.child_elements.push_back(std::move(child));

  steamrot::UIStyle style;
  style.panel_style.border_thickness = 0.0f;
  style.panel_style.inner_margin = {0.0f, 0.0f};

  // Act
  steamrot::logic::positioning::ui::PositionNestedUIElements(root, style);

  // Assert - root's child was positioned
  REQUIRE_THAT(root.child_elements[0]->size,
               steamrot::tests::EqualsVector2f({200.0f, 200.0f}));
  REQUIRE_THAT(root.child_elements[0]->position,
               steamrot::tests::EqualsVector2f({0.0f, 0.0f}));

  // grandchildren were also positioned by recursive call
  const auto &child_ref = *root.child_elements[0];
  REQUIRE_THAT(child_ref.child_elements[0]->size,
               steamrot::tests::EqualsVector2f({100.0f, 200.0f}));
  REQUIRE_THAT(child_ref.child_elements[1]->size,
               steamrot::tests::EqualsVector2f({100.0f, 200.0f}));
}

TEST_CASE("PositionNestedUIElements does not recurse into inactive children",
          "[unit][positioning_ui]") {
  // Arrange - parent with an inactive child
  steamrot::PanelElement root;
  root.position = {0.0f, 0.0f};
  root.size = {200.0f, 200.0f};
  root.layout = steamrot::Layout::Vertical;
  root.children_active = false; // children inactive

  auto child = std::make_unique<steamrot::PanelElement>();
  child->layout = steamrot::Layout::Horizontal;
  child->children_active = true;
  child->child_elements.push_back(std::make_unique<steamrot::PanelElement>());

  root.child_elements.push_back(std::move(child));

  steamrot::UIStyle style;
  style.panel_style.border_thickness = 0.0f;
  style.panel_style.inner_margin = {0.0f, 0.0f};

  // Act
  steamrot::logic::positioning::ui::PositionNestedUIElements(root, style);

  // Assert - root's child was still positioned by UpdateSizeAndPosition
  // but grandchild was not touched because children_active is false on root
  REQUIRE_THAT(root.child_elements[0]->size,
               steamrot::tests::EqualsVector2f({200.0f, 200.0f}));

  // grandchild should remain at default zero size (not positioned)
  REQUIRE_THAT(root.child_elements[0]->child_elements[0]->size,
               steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
}
