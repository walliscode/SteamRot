/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for collision detection functions in collision::mouse
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_mouse.h"
#include "PanelElement.h"
#include "catch2/generators/catch_generators.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("IsMouseOverBounds returns false for point outside bounds",
          "[unit][collision]") {
  sf::Vector2i mouse_position(150, 150);
  sf::FloatRect bounds({0, 0}, {100, 100});

  bool result =
      steamrot::logic::collision::mouse::IsMouseOverBounds(mouse_position, bounds);
  REQUIRE(result == false);
}

// Helper for easy construction
struct IsMouseOverTestCase {
  sf::Vector2i mouse_pos;
  sf::FloatRect bounds;
  bool expected;
};

TEST_CASE("IsMouseOverBounds returns correct results for various bounds and "
          "mouse positions",
          "[unit][collision]") {
  auto cases = GENERATE(
      IsMouseOverTestCase{{50, 50}, sf::FloatRect({0, 0}, {100, 100}), true},
      IsMouseOverTestCase{{0, 0}, sf::FloatRect({0, 0}, {100, 100}), true},
      IsMouseOverTestCase{{99, 99}, sf::FloatRect({0, 0}, {100, 100}), true},
      IsMouseOverTestCase{{100, 100}, sf::FloatRect({0, 0}, {100, 100}), false},
      IsMouseOverTestCase{{-1, 50}, sf::FloatRect({0, 0}, {100, 100}), false},
      IsMouseOverTestCase{{50, -1}, sf::FloatRect({0, 0}, {100, 100}), false},
      IsMouseOverTestCase{{150, 50}, sf::FloatRect({100, 0}, {50, 50}), false},
      IsMouseOverTestCase{{125, 25}, sf::FloatRect({100, 0}, {50, 50}), true},
      IsMouseOverTestCase{{149, 49}, sf::FloatRect({100, 0}, {50, 50}), true},
      IsMouseOverTestCase{{150, 50}, sf::FloatRect({100, 0}, {50, 50}), false},
      IsMouseOverTestCase{{0, 0}, sf::FloatRect({10, 10}, {30, 30}), false},
      IsMouseOverTestCase{{15, 15}, sf::FloatRect({10, 10}, {30, 30}), true},
      IsMouseOverTestCase{{40, 40}, sf::FloatRect({10, 10}, {30, 30}), false},
      IsMouseOverTestCase{{39, 39}, sf::FloatRect({10, 10}, {30, 30}), true},
      IsMouseOverTestCase{
          {200, 200}, sf::FloatRect({190, 190}, {10, 10}), false},
      IsMouseOverTestCase{
          {199, 199}, sf::FloatRect({190, 190}, {10, 10}), true},
      IsMouseOverTestCase{
          {189, 189}, sf::FloatRect({190, 190}, {10, 10}), false},
      IsMouseOverTestCase{
          {300, 400}, sf::FloatRect({250, 350}, {60, 70}), true},
      IsMouseOverTestCase{
          {310, 420}, sf::FloatRect({250, 350}, {60, 70}), false},
      IsMouseOverTestCase{
          {320, 430}, sf::FloatRect({250, 350}, {60, 70}), false});

  bool result = steamrot::logic::collision::mouse::IsMouseOverBounds(cases.mouse_pos,
                                                              cases.bounds);
  REQUIRE(result == cases.expected);
}

TEST_CASE("CheckMouseOver UIElement (no children) toggles is_mouse_over",
          "[unit][collision]") {

  // create Panel Element and set position and size
  steamrot::PanelElement panel_element;
  panel_element.position = {0, 0};
  panel_element.size = {100, 100};

  // ensure is_mouse_over is false initially
  REQUIRE(panel_element.is_mouse_over == false);
  // check mouse position inside bounds
  sf::Vector2i mouse_position(50, 50);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                    panel_element);
  REQUIRE(panel_element.is_mouse_over == true);
  // now move mouse outside bounds
  mouse_position = sf::Vector2i(150, 150);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                    panel_element);
  REQUIRE(panel_element.is_mouse_over == false);
}

TEST_CASE("CheckMouseOver UIElement toggles nested Panel Elements",
          "[unit][collision]") {
  // create parent Panel Element
  steamrot::PanelElement parent_element;
  parent_element.position = {0, 0};
  parent_element.size = {200, 200};

  // create child Panel Element
  auto child_element_to_move = std::make_unique<steamrot::PanelElement>();
  child_element_to_move->position = {50, 50};
  child_element_to_move->size = {100, 100};

  // add child to parent
  parent_element.child_elements.push_back(std::move(child_element_to_move));

  // get reference to the moved child for easy access
  auto &child_element = *static_cast<steamrot::PanelElement *>(
      parent_element.child_elements[0].get());

  // ensure both are not hovered initially
  REQUIRE(parent_element.is_mouse_over == false);
  REQUIRE(child_element.is_mouse_over == false);

  // move mouse over child
  sf::Vector2i mouse_position(75, 75);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                    parent_element);
  REQUIRE(child_element.is_mouse_over == true);
  REQUIRE(parent_element.is_mouse_over == false);

  // move mouse outside both
  mouse_position = sf::Vector2i(250, 250);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                    parent_element);
  REQUIRE(child_element.is_mouse_over == false);
  REQUIRE(parent_element.is_mouse_over == false);

  // move mouse over parent but outside child
  mouse_position = sf::Vector2i(175, 175);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                    parent_element);
  REQUIRE(child_element.is_mouse_over == false);
  REQUIRE(parent_element.is_mouse_over == true);
}

TEST_CASE("ClearMouseOver clears is_mouse_over on element and all children",
          "[unit][collision]") {
  steamrot::PanelElement parent;
  parent.position = {0, 0};
  parent.size = {200, 200};
  parent.is_mouse_over = true;

  auto child = std::make_unique<steamrot::PanelElement>();
  child->position = {50, 50};
  child->size = {100, 100};
  child->is_mouse_over = true;

  auto grandchild = std::make_unique<steamrot::PanelElement>();
  grandchild->position = {60, 60};
  grandchild->size = {20, 20};
  grandchild->is_mouse_over = true;

  child->child_elements.push_back(std::move(grandchild));
  parent.child_elements.push_back(std::move(child));

  // all hovered before clear
  REQUIRE(parent.is_mouse_over == true);
  REQUIRE(parent.child_elements[0]->is_mouse_over == true);
  REQUIRE(parent.child_elements[0]->child_elements[0]->is_mouse_over == true);

  steamrot::logic::collision::mouse::ClearMouseOver(parent);

  REQUIRE(parent.is_mouse_over == false);
  REQUIRE(parent.child_elements[0]->is_mouse_over == false);
  REQUIRE(parent.child_elements[0]->child_elements[0]->is_mouse_over == false);
}

TEST_CASE("AnyMouseOver returns true when any element in tree is hovered",
          "[unit][collision]") {
  steamrot::PanelElement parent;
  parent.position = {0, 0};
  parent.size = {200, 200};

  auto child = std::make_unique<steamrot::PanelElement>();
  child->position = {50, 50};
  child->size = {100, 100};

  parent.child_elements.push_back(std::move(child));

  SECTION("No elements hovered returns false") {
    REQUIRE(steamrot::logic::collision::mouse::AnyMouseOver(parent) == false);
  }

  SECTION("Root element hovered returns true") {
    parent.is_mouse_over = true;
    REQUIRE(steamrot::logic::collision::mouse::AnyMouseOver(parent) == true);
  }

  SECTION("Child element hovered returns true") {
    parent.child_elements[0]->is_mouse_over = true;
    REQUIRE(steamrot::logic::collision::mouse::AnyMouseOver(parent) == true);
  }
}

TEST_CASE("CheckMouseOver UIElement respects priority among overlapping sibling "
          "children",
          "[unit][collision]") {

  // Parent that covers the whole test area
  steamrot::PanelElement parent;
  parent.position = {0, 0};
  parent.size = {200, 200};

  // Both siblings occupy the same bounds so without priority they would both
  // be candidates for hover on the same mouse position
  auto low_priority = std::make_unique<steamrot::PanelElement>();
  low_priority->position = {50, 50};
  low_priority->size = {100, 100};
  low_priority->priority = 0;

  auto high_priority = std::make_unique<steamrot::PanelElement>();
  high_priority->position = {50, 50};
  high_priority->size = {100, 100};
  high_priority->priority = 10;

  // Insert low-priority first so that insertion order alone would give it
  // hover; priority must override this
  parent.child_elements.push_back(std::move(low_priority));
  parent.child_elements.push_back(std::move(high_priority));

  auto &low =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[0].get());
  auto &high =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[1].get());

  sf::Vector2i mouse_position(75, 75);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, parent);

  SECTION("Higher priority sibling receives hover") {
    REQUIRE(high.is_mouse_over == true);
  }

  SECTION("Lower priority sibling does not receive hover") {
    REQUIRE(low.is_mouse_over == false);
  }

  SECTION("Parent does not receive hover when a child is hovered") {
    REQUIRE(parent.is_mouse_over == false);
  }
}

TEST_CASE("CheckMouseOver UIElement selects lower priority sibling when higher "
          "priority is not under mouse",
          "[unit][collision]") {

  steamrot::PanelElement parent;
  parent.position = {0, 0};
  parent.size = {300, 300};

  // High priority child placed to the right - mouse will NOT be over it
  auto high_priority = std::make_unique<steamrot::PanelElement>();
  high_priority->position = {150, 0};
  high_priority->size = {100, 100};
  high_priority->priority = 10;

  // Low priority child on the left - mouse WILL be over it
  auto low_priority = std::make_unique<steamrot::PanelElement>();
  low_priority->position = {0, 0};
  low_priority->size = {100, 100};
  low_priority->priority = 0;

  parent.child_elements.push_back(std::move(low_priority));
  parent.child_elements.push_back(std::move(high_priority));

  auto &low =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[0].get());
  auto &high =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[1].get());

  // Mouse is over the low-priority child only
  sf::Vector2i mouse_position(50, 50);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, parent);

  REQUIRE(high.is_mouse_over == false);
  REQUIRE(low.is_mouse_over == true);
}

TEST_CASE("CheckMouseOver UIElement with high-priority container having nested "
          "hovered child blocks low-priority sibling",
          "[unit][collision]") {

  // Simulate the dropdown-over-exit-button scenario:
  //   root
  //   ├── exit_button       (priority 0)
  //   └── dropdown_container (priority 10)
  //         └── dropdown_item (position overlaps exit_button)
  //
  // When the mouse is over the dropdown_item, exit_button must NOT be hovered.

  steamrot::PanelElement root;
  root.position = {0, 0};
  root.size = {300, 300};

  // Exit button at the same position that the dropdown item will cover
  auto exit_button = std::make_unique<steamrot::PanelElement>();
  exit_button->position = {50, 50};
  exit_button->size = {100, 100};
  exit_button->priority = 0;

  // High-priority dropdown container that overlaps the exit button area
  auto dropdown_container = std::make_unique<steamrot::PanelElement>();
  dropdown_container->position = {0, 0};
  dropdown_container->size = {200, 200};
  dropdown_container->priority = 10;
  dropdown_container->children_active = true;

  // Dropdown item nested inside the container, overlapping the exit button
  auto dropdown_item = std::make_unique<steamrot::PanelElement>();
  dropdown_item->position = {50, 50};
  dropdown_item->size = {100, 100};
  dropdown_container->child_elements.push_back(std::move(dropdown_item));

  // Insert exit_button first (insertion order would give it hover without
  // priority)
  root.child_elements.push_back(std::move(exit_button));
  root.child_elements.push_back(std::move(dropdown_container));

  auto &exit_btn =
      *static_cast<steamrot::PanelElement *>(root.child_elements[0].get());
  auto &container =
      *static_cast<steamrot::PanelElement *>(root.child_elements[1].get());
  auto &item = *static_cast<steamrot::PanelElement *>(
      container.child_elements[0].get());

  // Mouse is over the area shared by exit_button and dropdown_item
  sf::Vector2i mouse_position(75, 75);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, root);

  SECTION("Nested dropdown item receives hover") {
    REQUIRE(item.is_mouse_over == true);
  }

  SECTION("Exit button (lower-priority sibling of container) is not hovered") {
    REQUIRE(exit_btn.is_mouse_over == false);
  }

  SECTION("Dropdown container itself is not hovered (descendant is)") {
    REQUIRE(container.is_mouse_over == false);
  }

  SECTION("Root is not hovered") {
    REQUIRE(root.is_mouse_over == false);
  }
}
