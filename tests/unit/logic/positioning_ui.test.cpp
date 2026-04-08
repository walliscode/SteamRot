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
#include <memory>

// ---------------------------------------------------------------------------
// CalculateAvailableSize
// ---------------------------------------------------------------------------

TEST_CASE("CalculateAvailableSize with border only", "[unit][positioning_ui]") {
  sf::Vector2f size{200.0f, 100.0f};
  // available = size - 2 * border on each axis
  REQUIRE_THAT(
      steamrot::logic::positioning::ui::CalculateAvailableSize(size, 5.0f),
      steamrot::tests::EqualsVector2f({190.0f, 90.0f}));
}

TEST_CASE("CalculateAvailableSize with border and margin",
          "[unit][positioning_ui]") {
  sf::Vector2f size{200.0f, 100.0f};
  sf::Vector2f margin{10.0f, 8.0f};
  // available = size - 2*border - 2*margin on each axis
  REQUIRE_THAT(steamrot::logic::positioning::ui::CalculateAvailableSize(
                   size, 5.0f, margin),
               steamrot::tests::EqualsVector2f({170.0f, 74.0f}));
}

TEST_CASE("CalculateAvailableSize with zero border and margin",
          "[unit][positioning_ui]") {
  sf::Vector2f size{300.0f, 150.0f};
  REQUIRE_THAT(
      steamrot::logic::positioning::ui::CalculateAvailableSize(size, 0.0f),
      steamrot::tests::EqualsVector2f({300.0f, 150.0f}));
}

// ---------------------------------------------------------------------------
// CalculateStartPosition
// ---------------------------------------------------------------------------

TEST_CASE("CalculateStartPosition with border only", "[unit][positioning_ui]") {
  sf::Vector2f position{20.0f, 10.0f};
  REQUIRE_THAT(
      steamrot::logic::positioning::ui::CalculateStartPosition(position, 5.0f),
      steamrot::tests::EqualsVector2f({25.0f, 15.0f}));
}

TEST_CASE("CalculateStartPosition with border and margin",
          "[unit][positioning_ui]") {
  sf::Vector2f position{20.0f, 10.0f};
  sf::Vector2f margin{10.0f, 8.0f};
  REQUIRE_THAT(steamrot::logic::positioning::ui::CalculateStartPosition(
                   position, 5.0f, margin),
               steamrot::tests::EqualsVector2f({35.0f, 23.0f}));
}

TEST_CASE("CalculateStartPosition with zero border and margin",
          "[unit][positioning_ui]") {
  sf::Vector2f position{50.0f, 30.0f};
  REQUIRE_THAT(
      steamrot::logic::positioning::ui::CalculateStartPosition(position, 0.0f),
      steamrot::tests::EqualsVector2f({50.0f, 30.0f}));
}

// ---------------------------------------------------------------------------
// PositionDropDownContainerChildren
// ---------------------------------------------------------------------------

TEST_CASE("PositionDropDownContainerChildren positions list and button "
          "children correctly",
          "[unit][positioning_ui]") {
  steamrot::DropDownContainerElement container;
  container.position = {0.0f, 0.0f};
  container.size = {200.0f, 50.0f};
  container.child_elements.push_back(
      std::make_unique<steamrot::DropDownListElement>());
  container.child_elements.push_back(
      std::make_unique<steamrot::DropDownButtonElement>());

  steamrot::UIStyle style;
  style.drop_down_container_style.border_thickness = 2.0f;
  style.drop_down_container_style.drop_symbol_ratio = 0.2f;

  steamrot::logic::positioning::ui::PositionDropDownContainerChildren(container,
                                                                      style);

  // available = {200 - 4, 50 - 4} = {196, 46}
  // list_size  = {196 * 0.8, 46} = {156.8, 46}
  // button_size = {196 * 0.2, 46} = {39.2, 46}
  // list_pos   = {2, 2}
  // button_pos = {2 + 156.8, 2} = {158.8, 2}
  float available_width = 200.0f - 2 * 2.0f;
  float available_height = 50.0f - 2 * 2.0f;
  float ratio = 0.2f;
  float list_width = available_width * (1.0f - ratio);
  float button_width = available_width * ratio;

  REQUIRE_THAT(container.child_elements[0]->size,
               steamrot::tests::EqualsVector2f({list_width, available_height}));
  REQUIRE_THAT(container.child_elements[0]->position,
               steamrot::tests::EqualsVector2f({2.0f, 2.0f}));

  REQUIRE_THAT(
      container.child_elements[1]->size,
      steamrot::tests::EqualsVector2f({button_width, available_height}));
  REQUIRE_THAT(container.child_elements[1]->position,
               steamrot::tests::EqualsVector2f({2.0f + list_width, 2.0f}));
}

TEST_CASE("PositionDropDownContainerChildren handles missing second child",
          "[unit][positioning_ui]") {
  steamrot::DropDownContainerElement container;
  container.position = {0.0f, 0.0f};
  container.size = {100.0f, 40.0f};
  container.child_elements.push_back(
      std::make_unique<steamrot::DropDownListElement>());

  steamrot::UIStyle style;
  style.drop_down_container_style.border_thickness = 0.0f;
  style.drop_down_container_style.drop_symbol_ratio = 0.25f;

  // Should not crash with only one child
  REQUIRE_NOTHROW(
      steamrot::logic::positioning::ui::PositionDropDownContainerChildren(
          container, style));

  REQUIRE_THAT(container.child_elements[0]->size,
               steamrot::tests::EqualsVector2f({75.0f, 40.0f}));
}

TEST_CASE("PositionDropDownContainerChildren handles empty child list",
          "[unit][positioning_ui]") {
  steamrot::DropDownContainerElement container;
  container.position = {0.0f, 0.0f};
  container.size = {100.0f, 40.0f};

  steamrot::UIStyle style;
  style.drop_down_container_style.border_thickness = 2.0f;
  style.drop_down_container_style.drop_symbol_ratio = 0.2f;

  // Should not crash with no children
  REQUIRE_NOTHROW(
      steamrot::logic::positioning::ui::PositionDropDownContainerChildren(
          container, style));
}

// ---------------------------------------------------------------------------
// PositionVerticalLayoutChildren
// ---------------------------------------------------------------------------

TEST_CASE("PositionVerticalLayoutChildren does nothing when element has no "
          "children",
          "[unit][positioning_ui]") {
  steamrot::PanelElement parent;
  parent.position = {10.0f, 10.0f};
  parent.size = {200.0f, 100.0f};
  parent.layout = steamrot::Layout::Vertical;

  steamrot::UIStyle style;
  style.panel_style.border_thickness = 5.0f;
  style.panel_style.inner_margin = {10.0f, 10.0f};

  REQUIRE_NOTHROW(
      steamrot::logic::positioning::ui::PositionVerticalLayoutChildren(parent,
                                                                       style));
}

TEST_CASE("PositionVerticalLayoutChildren divides height equally among "
          "children",
          "[unit][positioning_ui]") {
  steamrot::PanelElement parent;
  parent.position = {0.0f, 0.0f};
  parent.size = {200.0f, 300.0f};
  parent.layout = steamrot::Layout::Vertical;
  parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());
  parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());

  steamrot::UIStyle style;
  style.panel_style.border_thickness = 5.0f;
  style.panel_style.inner_margin = {10.0f, 10.0f};

  steamrot::logic::positioning::ui::PositionVerticalLayoutChildren(parent,
                                                                   style);

  // available = {200-10-20, 300-10-20} = {170, 270}
  // start     = {0+5+10, 0+5+10}       = {15, 15}
  // height    = (270 - 1*10) / 2       = 130
  float child_height = (270.0f - 10.0f) / 2.0f;

  REQUIRE_THAT(parent.child_elements[0]->size,
               steamrot::tests::EqualsVector2f({170.0f, child_height}));
  REQUIRE_THAT(parent.child_elements[0]->position,
               steamrot::tests::EqualsVector2f({15.0f, 15.0f}));

  REQUIRE_THAT(parent.child_elements[1]->size,
               steamrot::tests::EqualsVector2f({170.0f, child_height}));
  REQUIRE_THAT(
      parent.child_elements[1]->position,
      steamrot::tests::EqualsVector2f({15.0f, 15.0f + child_height + 10.0f}));
}

TEST_CASE("PositionVerticalLayoutChildren positions a single child correctly",
          "[unit][positioning_ui]") {
  steamrot::PanelElement parent;
  parent.position = {10.0f, 20.0f};
  parent.size = {100.0f, 80.0f};
  parent.layout = steamrot::Layout::Vertical;
  parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());

  steamrot::UIStyle style;
  style.panel_style.border_thickness = 0.0f;
  style.panel_style.inner_margin = {0.0f, 0.0f};

  steamrot::logic::positioning::ui::PositionVerticalLayoutChildren(parent,
                                                                   style);

  // single child fills available space entirely
  REQUIRE_THAT(parent.child_elements[0]->size,
               steamrot::tests::EqualsVector2f({100.0f, 80.0f}));
  REQUIRE_THAT(parent.child_elements[0]->position,
               steamrot::tests::EqualsVector2f({10.0f, 20.0f}));
}

// ---------------------------------------------------------------------------
// PositionHorizontalLayoutChildren
// ---------------------------------------------------------------------------

TEST_CASE("PositionHorizontalLayoutChildren does nothing when element has no "
          "children",
          "[unit][positioning_ui]") {
  steamrot::PanelElement parent;
  parent.position = {10.0f, 10.0f};
  parent.size = {200.0f, 100.0f};
  parent.layout = steamrot::Layout::Horizontal;

  steamrot::UIStyle style;
  style.panel_style.border_thickness = 5.0f;
  style.panel_style.inner_margin = {10.0f, 10.0f};

  REQUIRE_NOTHROW(
      steamrot::logic::positioning::ui::PositionHorizontalLayoutChildren(
          parent, style));
}

TEST_CASE("PositionHorizontalLayoutChildren divides width equally among "
          "children",
          "[unit][positioning_ui]") {
  steamrot::PanelElement parent;
  parent.position = {0.0f, 0.0f};
  parent.size = {300.0f, 100.0f};
  parent.layout = steamrot::Layout::Horizontal;
  parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());
  parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());

  steamrot::UIStyle style;
  style.panel_style.border_thickness = 5.0f;
  style.panel_style.inner_margin = {10.0f, 10.0f};

  steamrot::logic::positioning::ui::PositionHorizontalLayoutChildren(parent,
                                                                     style);

  // available = {300-10-20, 100-10-20} = {270, 70}
  // start     = {15, 15}
  // width     = (270 - 1*10) / 2       = 130
  float child_width = (270.0f - 10.0f) / 2.0f;

  REQUIRE_THAT(parent.child_elements[0]->size,
               steamrot::tests::EqualsVector2f({child_width, 70.0f}));
  REQUIRE_THAT(parent.child_elements[0]->position,
               steamrot::tests::EqualsVector2f({15.0f, 15.0f}));

  REQUIRE_THAT(parent.child_elements[1]->size,
               steamrot::tests::EqualsVector2f({child_width, 70.0f}));
  REQUIRE_THAT(
      parent.child_elements[1]->position,
      steamrot::tests::EqualsVector2f({15.0f + child_width + 10.0f, 15.0f}));
}

TEST_CASE("PositionHorizontalLayoutChildren positions a single child correctly",
          "[unit][positioning_ui]") {
  steamrot::PanelElement parent;
  parent.position = {5.0f, 5.0f};
  parent.size = {120.0f, 60.0f};
  parent.layout = steamrot::Layout::Horizontal;
  parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());

  steamrot::UIStyle style;
  style.panel_style.border_thickness = 0.0f;
  style.panel_style.inner_margin = {0.0f, 0.0f};

  steamrot::logic::positioning::ui::PositionHorizontalLayoutChildren(parent,
                                                                     style);

  REQUIRE_THAT(parent.child_elements[0]->size,
               steamrot::tests::EqualsVector2f({120.0f, 60.0f}));
  REQUIRE_THAT(parent.child_elements[0]->position,
               steamrot::tests::EqualsVector2f({5.0f, 5.0f}));
}

// ---------------------------------------------------------------------------
// PositionDropDownLayoutChildren
// ---------------------------------------------------------------------------
TEST_CASE("PositionDropDownListChildren responds to various scenarious",
          "[unit][positioning_ui]") {

  // Arrange
  std::vector<std::unique_ptr<steamrot::UIElement>> children;
  steamrot::UIStyle style;
  sf::Vector2f available_size{300.0f, 15.0f};
  sf::Vector2f start_position{10.0f, 20.0f};

  std::unique_ptr<steamrot::DropDownItemElement> item1 =
      std::make_unique<steamrot::DropDownItemElement>();
  std::unique_ptr<steamrot::DropDownItemElement> item2 =
      std::make_unique<steamrot::DropDownItemElement>();
  std::unique_ptr<steamrot::DropDownItemElement> item3 =
      std::make_unique<steamrot::DropDownItemElement>();

  REQUIRE(item1);
  REQUIRE(item1->size == sf::Vector2f{0.0f, 0.0f});
  REQUIRE(item1->position == sf::Vector2f{0.0f, 0.0f});
  REQUIRE(item2);
  REQUIRE(item2->size == sf::Vector2f{0.0f, 0.0f});
  REQUIRE(item2->position == sf::Vector2f{0.0f, 0.0f});
  REQUIRE(item3);
  REQUIRE(item3->size == sf::Vector2f{0.0f, 0.0f});
  REQUIRE(item3->position == sf::Vector2f{0.0f, 0.0f});

  SECTION("Empty child list") {
    // Act & Assert - should not crash with empty child list
    REQUIRE_NOTHROW(
        steamrot::logic::positioning::ui::PositionDropDownListChildren(
            children, available_size, start_position, style));
  }

  SECTION("Stacks single child at provided start position") {
    children.push_back(std::move(item1));
    steamrot::logic::positioning::ui::PositionDropDownListChildren(
        children, available_size, start_position, style);

    REQUIRE_THAT(children[0]->size,
                 steamrot::tests::EqualsVector2f(available_size));
    REQUIRE_THAT(children[0]->position,
                 steamrot::tests::EqualsVector2f(start_position));
  }

  SECTION("Stacks multiple children vertically with no spacing, each child "
          "being of full available height") {
    children.push_back(std::move(item1));
    children.push_back(std::move(item2));
    children.push_back(std::move(item3));
    steamrot::logic::positioning::ui::PositionDropDownListChildren(
        children, available_size, start_position, style);

    REQUIRE_THAT(children[0]->size,
                 steamrot::tests::EqualsVector2f(available_size));
    REQUIRE_THAT(children[0]->position,
                 steamrot::tests::EqualsVector2f(start_position));

    REQUIRE_THAT(children[1]->size,
                 steamrot::tests::EqualsVector2f(available_size));
    REQUIRE_THAT(children[1]->position,
                 steamrot::tests::EqualsVector2f(
                     start_position + sf::Vector2f{0.0f, available_size.y}));

    REQUIRE_THAT(children[2]->size,
                 steamrot::tests::EqualsVector2f(available_size));
    REQUIRE_THAT(
        children[2]->position,
        steamrot::tests::EqualsVector2f(
            start_position + sf::Vector2f{0.0f, 2 * available_size.y}));
  }
}
// ---------------------------------------------------------------------------
// UpdateSizeAndPositionOfChildElements (existing tests preserved)
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// PositionNestedUIElements (existing tests preserved)
// ---------------------------------------------------------------------------

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
