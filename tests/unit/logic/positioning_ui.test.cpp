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

TEST_CASE("CalculateAvailableSize", "[unit][positioning_ui]") {

  SECTION("with border only") {
    sf::Vector2f size{200.0f, 100.0f};
    // available = size - 2 * border on each axis
    REQUIRE_THAT(
        steamrot::logic::positioning::ui::CalculateAvailableSize(size, 5.0f),
        steamrot::tests::EqualsVector2f({190.0f, 90.0f}));
  }

  SECTION("with border and margin") {
    sf::Vector2f size{200.0f, 100.0f};
    sf::Vector2f margin{10.0f, 8.0f};
    // available = size - 2*border - 2*margin on each axis
    REQUIRE_THAT(steamrot::logic::positioning::ui::CalculateAvailableSize(
                     size, 5.0f, margin),
                 steamrot::tests::EqualsVector2f({170.0f, 74.0f}));
  }

  SECTION("with zero border and margin") {
    sf::Vector2f size{300.0f, 150.0f};
    REQUIRE_THAT(
        steamrot::logic::positioning::ui::CalculateAvailableSize(size, 0.0f),
        steamrot::tests::EqualsVector2f({300.0f, 150.0f}));
  }
}

// ---------------------------------------------------------------------------
// CalculateStartPosition
// ---------------------------------------------------------------------------

TEST_CASE("CalculateStartPosition", "[unit][positioning_ui]") {

  SECTION("with border only") {
    sf::Vector2f position{20.0f, 10.0f};
    REQUIRE_THAT(steamrot::logic::positioning::ui::CalculateStartPosition(
                     position, 5.0f),
                 steamrot::tests::EqualsVector2f({25.0f, 15.0f}));
  }

  SECTION("with border and margin") {
    sf::Vector2f position{20.0f, 10.0f};
    sf::Vector2f margin{10.0f, 8.0f};
    REQUIRE_THAT(steamrot::logic::positioning::ui::CalculateStartPosition(
                     position, 5.0f, margin),
                 steamrot::tests::EqualsVector2f({35.0f, 23.0f}));
  }

  SECTION("with zero border and margin") {
    sf::Vector2f position{50.0f, 30.0f};
    REQUIRE_THAT(steamrot::logic::positioning::ui::CalculateStartPosition(
                     position, 0.0f),
                 steamrot::tests::EqualsVector2f({50.0f, 30.0f}));
  }
}

// ---------------------------------------------------------------------------
// PositionDropDownContainerChildren
// ---------------------------------------------------------------------------

TEST_CASE("PositionDropDownContainerChildren", "[unit][positioning_ui]") {

  SECTION("positions list and button children correctly") {
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

    steamrot::logic::positioning::ui::PositionDropDownContainerChildren(
        container, style);

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

    REQUIRE_THAT(
        container.child_elements[0]->size,
        steamrot::tests::EqualsVector2f({list_width, available_height}));
    REQUIRE_THAT(container.child_elements[0]->position,
                 steamrot::tests::EqualsVector2f({2.0f, 2.0f}));

    REQUIRE_THAT(
        container.child_elements[1]->size,
        steamrot::tests::EqualsVector2f({button_width, available_height}));
    REQUIRE_THAT(container.child_elements[1]->position,
                 steamrot::tests::EqualsVector2f({2.0f + list_width, 2.0f}));
  }

  SECTION("handles missing second child") {
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

  SECTION("handles empty child list") {
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

  SECTION("positions DropDownItemElement children via "
          "PositionDropDownListChildren") {
    // container: position={0,0}, size={200,60}, border=0, ratio=0.2
    // list_size = {200*0.8, 60} = {160, 60}, list_pos = {0, 0}
    // with 2 items: item_height = 60/2 = 30
    // item[0]: position={0,0}, size={160,30}
    // item[1]: position={0,30}, size={160,30}
    steamrot::DropDownContainerElement container;
    container.position = {0.0f, 0.0f};
    container.size = {200.0f, 30.0f};

    auto list = std::make_unique<steamrot::DropDownListElement>();
    list->child_elements.push_back(
        std::make_unique<steamrot::DropDownItemElement>());
    list->child_elements.push_back(
        std::make_unique<steamrot::DropDownItemElement>());
    container.child_elements.push_back(std::move(list));
    container.child_elements.push_back(
        std::make_unique<steamrot::DropDownButtonElement>());

    steamrot::UIStyle style;
    style.drop_down_container_style.border_thickness = 0.0f;
    style.drop_down_container_style.drop_symbol_ratio = 0.2f;

    auto available_size =
        steamrot::logic::positioning::ui::CalculateAvailableSize(
            container.size, style.drop_down_container_style.border_thickness);

    auto dd_list_size = sf::Vector2f{
        available_size.x *
            (1 - style.drop_down_container_style.drop_symbol_ratio),
        available_size.y};

    steamrot::logic::positioning::ui::PositionDropDownContainerChildren(
        container, style);

    const auto &list_children = container.child_elements[0]->child_elements;

    // DropDownItemElement children start below the DropDownContainerElement
    // position, not the DropDownListElement position itself
    REQUIRE_THAT(list_children[0]->size,
                 steamrot::tests::EqualsVector2f(dd_list_size));
    REQUIRE_THAT(
        list_children[0]->position,
        steamrot::tests::EqualsVector2f({0.0f, 0.0f + (dd_list_size.y * 1)}));

    REQUIRE_THAT(list_children[1]->size,
                 steamrot::tests::EqualsVector2f(dd_list_size));
    REQUIRE_THAT(
        list_children[1]->position,
        steamrot::tests::EqualsVector2f({0.0f, 0.0f + (dd_list_size.y * 2)}));
  }
}

// ---------------------------------------------------------------------------
// PositionVerticalLayoutChildren
// ---------------------------------------------------------------------------

TEST_CASE("PositionVerticalLayoutChildren", "[unit][positioning_ui]") {

  SECTION("does nothing when element has no children") {
    steamrot::PanelElement parent;
    parent.position = {10.0f, 10.0f};
    parent.size = {200.0f, 100.0f};
    parent.layout = steamrot::Layout::Vertical;

    steamrot::UIStyle style;
    style.panel_style.border_thickness = 5.0f;
    style.panel_style.inner_margin = {10.0f, 10.0f};

    REQUIRE_NOTHROW(
        steamrot::logic::positioning::ui::PositionVerticalLayoutChildren(
            parent, style));
  }

  SECTION("divides height equally among children") {
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

  SECTION("positions a single child correctly") {
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

  SECTION("does not position DropDownContainerElement children") {
    // DropDownContainerElement has a specialist positioning function
    // (PositionDropDownContainerChildren); the generic layout functions should
    // not be dispatched for it. UpdateSizeAndPositionOfChildElements only
    // processes PanelElement types, so DropDownContainerElement children are
    // untouched.
    steamrot::DropDownContainerElement container;
    container.position = {0.0f, 0.0f};
    container.size = {200.0f, 50.0f};
    container.child_elements.push_back(
        std::make_unique<steamrot::DropDownListElement>());
    container.child_elements.push_back(
        std::make_unique<steamrot::DropDownButtonElement>());

    steamrot::UIStyle style;
    style.panel_style.border_thickness = 5.0f;
    style.panel_style.inner_margin = {10.0f, 10.0f};

    steamrot::logic::positioning::ui::UpdateSizeAndPositionOfChildElements(
        container, style);

    // Children should remain at their default zero position/size — the generic
    // vertical layout was not applied because Layout::DropDown is a no-op in
    // UpdateSizeAndPositionOfChildElements.
    REQUIRE_THAT(container.child_elements[0]->size,
                 steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
    REQUIRE_THAT(container.child_elements[0]->position,
                 steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
    REQUIRE_THAT(container.child_elements[1]->size,
                 steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
    REQUIRE_THAT(container.child_elements[1]->position,
                 steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
  }
}

// ---------------------------------------------------------------------------
// PositionHorizontalLayoutChildren
// ---------------------------------------------------------------------------

TEST_CASE("PositionHorizontalLayoutChildren", "[unit][positioning_ui]") {

  SECTION("does nothing when element has no children") {
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

  SECTION("divides width equally among children") {
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

  SECTION("positions a single child correctly") {
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

  SECTION("does not position DropDownContainerElement children") {
    // DropDownContainerElement has a specialist positioning function
    // (PositionDropDownContainerChildren); the generic layout functions should
    // not be dispatched for it. UpdateSizeAndPositionOfChildElements only
    // processes PanelElement types, so DropDownContainerElement children are
    // untouched.
    steamrot::DropDownContainerElement container;
    container.position = {0.0f, 0.0f};
    container.size = {300.0f, 100.0f};
    container.child_elements.push_back(
        std::make_unique<steamrot::DropDownListElement>());
    container.child_elements.push_back(
        std::make_unique<steamrot::DropDownButtonElement>());

    steamrot::UIStyle style;
    style.panel_style.border_thickness = 5.0f;
    style.panel_style.inner_margin = {10.0f, 10.0f};

    steamrot::logic::positioning::ui::UpdateSizeAndPositionOfChildElements(
        container, style);

    // Children should remain at their default zero position/size — the generic
    // layout was not applied because DropDownContainerElement is not a
    // PanelElement.
    REQUIRE_THAT(container.child_elements[0]->size,
                 steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
    REQUIRE_THAT(container.child_elements[0]->position,
                 steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
    REQUIRE_THAT(container.child_elements[1]->size,
                 steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
    REQUIRE_THAT(container.child_elements[1]->position,
                 steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
  }
}

// ---------------------------------------------------------------------------
// PositionDropDownListChildren
// ---------------------------------------------------------------------------

TEST_CASE("PositionDropDownListChildren", "[unit][positioning_ui]") {

  std::vector<std::unique_ptr<steamrot::UIElement>> children;
  steamrot::UIStyle style;
  sf::Vector2f available_size{300.0f, 15.0f};
  sf::Vector2f start_position{10.0f, 20.0f};

  SECTION("does nothing with empty child list") {
    REQUIRE_NOTHROW(
        steamrot::logic::positioning::ui::PositionDropDownListChildren(
            children, available_size, start_position, style));
  }

  SECTION("stacks single child at provided start position") {
    children.push_back(std::make_unique<steamrot::DropDownItemElement>());

    steamrot::logic::positioning::ui::PositionDropDownListChildren(
        children, available_size, start_position, style);

    REQUIRE_THAT(children[0]->size,
                 steamrot::tests::EqualsVector2f(available_size));
    REQUIRE_THAT(children[0]->position,
                 steamrot::tests::EqualsVector2f(start_position));
  }

  SECTION("stacks multiple children vertically with no spacing, each child "
          "taking full available width and height") {
    children.push_back(std::make_unique<steamrot::DropDownItemElement>());
    children.push_back(std::make_unique<steamrot::DropDownItemElement>());
    children.push_back(std::make_unique<steamrot::DropDownItemElement>());

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
// UpdateSizeAndPositionOfChildElements
// ---------------------------------------------------------------------------

TEST_CASE("UpdateSizeAndPositionOfChildElements", "[unit][positioning_ui]") {

  SECTION("does nothing when element has no children") {
    steamrot::PanelElement panel;
    panel.position = {10.0f, 10.0f};
    panel.size = {200.0f, 100.0f};

    steamrot::UIStyle style;

    steamrot::logic::positioning::ui::UpdateSizeAndPositionOfChildElements(
        panel, style);

    REQUIRE_THAT(panel.position,
                 steamrot::tests::EqualsVector2f({10.0f, 10.0f}));
    REQUIRE_THAT(panel.size, steamrot::tests::EqualsVector2f({200.0f, 100.0f}));
  }

  SECTION("positions Vertical layout children correctly") {
    steamrot::PanelElement parent;
    parent.position = {0.0f, 0.0f};
    parent.size = {200.0f, 300.0f};
    parent.layout = steamrot::Layout::Vertical;
    parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());
    parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());

    steamrot::UIStyle style;
    style.panel_style.border_thickness = 5.0f;
    style.panel_style.inner_margin = {10.0f, 10.0f};

    steamrot::logic::positioning::ui::UpdateSizeAndPositionOfChildElements(
        parent, style);

    // available_width = 200 - 2*5 - 2*10 = 170
    // available_height = 300 - 2*5 - 2*10 = 270
    // child_height = (270 - 1*10) / 2 = 130
    // start_position = {0+5+10, 0+5+10} = {15, 15}
    float expected_width = 200.0f - 2 * 5.0f - 2 * 10.0f;
    float expected_height = 300.0f - 2 * 5.0f - 2 * 10.0f;
    float child_height = (expected_height - 1 * 10.0f) / 2.0f;

    REQUIRE_THAT(
        parent.child_elements[0]->size,
        steamrot::tests::EqualsVector2f({expected_width, child_height}));
    REQUIRE_THAT(parent.child_elements[0]->position,
                 steamrot::tests::EqualsVector2f({15.0f, 15.0f}));

    REQUIRE_THAT(
        parent.child_elements[1]->size,
        steamrot::tests::EqualsVector2f({expected_width, child_height}));
    REQUIRE_THAT(
        parent.child_elements[1]->position,
        steamrot::tests::EqualsVector2f({15.0f, 15.0f + child_height + 10.0f}));
  }

  SECTION("positions Horizontal layout children correctly") {
    steamrot::PanelElement parent;
    parent.position = {0.0f, 0.0f};
    parent.size = {300.0f, 100.0f};
    parent.layout = steamrot::Layout::Horizontal;
    parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());
    parent.child_elements.push_back(std::make_unique<steamrot::PanelElement>());

    steamrot::UIStyle style;
    style.panel_style.border_thickness = 5.0f;
    style.panel_style.inner_margin = {10.0f, 10.0f};

    steamrot::logic::positioning::ui::UpdateSizeAndPositionOfChildElements(
        parent, style);

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

  SECTION("does not position DropDownContainerElement children (specialist "
          "approach)") {
    // DropDownContainerElement uses PositionDropDownContainerChildren as its
    // specialist approach. UpdateSizeAndPositionOfChildElements only processes
    // PanelElement types, so DropDownContainerElement children are untouched.
    steamrot::DropDownContainerElement container;
    container.position = {0.0f, 0.0f};
    container.size = {200.0f, 50.0f};
    container.child_elements.push_back(
        std::make_unique<steamrot::DropDownListElement>());
    container.child_elements.push_back(
        std::make_unique<steamrot::DropDownButtonElement>());

    steamrot::UIStyle style;
    style.panel_style.border_thickness = 5.0f;
    style.panel_style.inner_margin = {10.0f, 10.0f};

    steamrot::logic::positioning::ui::UpdateSizeAndPositionOfChildElements(
        container, style);

    // Children must remain at default zero values — no generic layout applied.
    REQUIRE_THAT(container.child_elements[0]->size,
                 steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
    REQUIRE_THAT(container.child_elements[0]->position,
                 steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
    REQUIRE_THAT(container.child_elements[1]->size,
                 steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
    REQUIRE_THAT(container.child_elements[1]->position,
                 steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
  }
}

// ---------------------------------------------------------------------------
// PositionNestedUIElements
// ---------------------------------------------------------------------------

TEST_CASE("PositionNestedUIElements", "[unit][positioning_ui]") {

  SECTION("recursively positions active children") {
    // parent with one active child that itself has children
    steamrot::PanelElement root;
    root.position = {0.0f, 0.0f};
    root.size = {200.0f, 200.0f};
    root.layout = steamrot::Layout::Vertical;
    root.children_active = true;

    auto child = std::make_unique<steamrot::PanelElement>();
    child->layout = steamrot::Layout::Horizontal;
    child->children_active = true;

    // grandchildren — will be positioned by child's UpdateSizeAndPosition call
    child->child_elements.push_back(std::make_unique<steamrot::PanelElement>());
    child->child_elements.push_back(std::make_unique<steamrot::PanelElement>());

    root.child_elements.push_back(std::move(child));

    steamrot::UIStyle style;
    style.panel_style.border_thickness = 0.0f;
    style.panel_style.inner_margin = {0.0f, 0.0f};

    steamrot::logic::positioning::ui::PositionNestedUIElements(root, style);

    // root's child was positioned
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

  SECTION("does not recurse into inactive children") {
    // parent with an inactive child
    steamrot::PanelElement root;
    root.position = {0.0f, 0.0f};
    root.size = {200.0f, 200.0f};
    root.layout = steamrot::Layout::Vertical;
    root.children_active = false;

    auto child = std::make_unique<steamrot::PanelElement>();
    child->layout = steamrot::Layout::Horizontal;
    child->children_active = true;
    child->child_elements.push_back(std::make_unique<steamrot::PanelElement>());

    root.child_elements.push_back(std::move(child));

    steamrot::UIStyle style;
    style.panel_style.border_thickness = 0.0f;
    style.panel_style.inner_margin = {0.0f, 0.0f};

    steamrot::logic::positioning::ui::PositionNestedUIElements(root, style);

    // root's child was still positioned by UpdateSizeAndPosition
    // but grandchild was not touched because children_active is false on root
    REQUIRE_THAT(root.child_elements[0]->size,
                 steamrot::tests::EqualsVector2f({200.0f, 200.0f}));

    // grandchild should remain at default zero size (not positioned)
    REQUIRE_THAT(root.child_elements[0]->child_elements[0]->size,
                 steamrot::tests::EqualsVector2f({0.0f, 0.0f}));
  }
}
