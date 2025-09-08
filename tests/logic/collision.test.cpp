/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for collsion detection functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision.h"
#include "PanelElement.h"
#include "catch2/generators/catch_generators.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("IsMouseOverBounds returns false for point outside bounds",
          "[collision]") {
  sf::Vector2i mouse_position(150, 150);
  sf::FloatRect bounds({0, 0}, {100, 100});

  bool result = steamrot::collision::IsMouseOverBounds(mouse_position, bounds);
  REQUIRE(result == false);
}

// Helper for easy construction
struct TestCase {
  sf::Vector2i mouse_pos;
  sf::FloatRect bounds;
  bool expected;
};

TEST_CASE("IsMouseOverBounds returns correct results for various bounds and "
          "mouse positions",
          "[collision]") {
  auto cases = GENERATE(
      TestCase{{50, 50}, sf::FloatRect({0, 0}, {100, 100}), true},
      TestCase{{0, 0}, sf::FloatRect({0, 0}, {100, 100}), true},
      TestCase{{99, 99}, sf::FloatRect({0, 0}, {100, 100}), true},
      TestCase{{100, 100}, sf::FloatRect({0, 0}, {100, 100}), false},
      TestCase{{-1, 50}, sf::FloatRect({0, 0}, {100, 100}), false},
      TestCase{{50, -1}, sf::FloatRect({0, 0}, {100, 100}), false},
      TestCase{{150, 50}, sf::FloatRect({100, 0}, {50, 50}), false},
      TestCase{{125, 25}, sf::FloatRect({100, 0}, {50, 50}), true},
      TestCase{{149, 49}, sf::FloatRect({100, 0}, {50, 50}), true},
      TestCase{{150, 50}, sf::FloatRect({100, 0}, {50, 50}), false},
      TestCase{{0, 0}, sf::FloatRect({10, 10}, {30, 30}), false},
      TestCase{{15, 15}, sf::FloatRect({10, 10}, {30, 30}), true},
      TestCase{{40, 40}, sf::FloatRect({10, 10}, {30, 30}), false},
      TestCase{{39, 39}, sf::FloatRect({10, 10}, {30, 30}), true},
      TestCase{{200, 200}, sf::FloatRect({190, 190}, {10, 10}), false},
      TestCase{{199, 199}, sf::FloatRect({190, 190}, {10, 10}), true},
      TestCase{{189, 189}, sf::FloatRect({190, 190}, {10, 10}), false},
      TestCase{{300, 400}, sf::FloatRect({250, 350}, {60, 70}), true},
      TestCase{{310, 420}, sf::FloatRect({250, 350}, {60, 70}), false},
      TestCase{{320, 430}, sf::FloatRect({250, 350}, {60, 70}), false});

  CAPTURE(cases.mouse_pos.x, cases.mouse_pos.y,
          static_cast<float>(cases.mouse_pos.x),
          static_cast<float>(cases.mouse_pos.y), cases.bounds.position.x,
          cases.bounds.position.y,
          (cases.bounds.position.x + cases.bounds.size.x), cases.expected);
  bool result =
      steamrot::collision::IsMouseOverBounds(cases.mouse_pos, cases.bounds);
  REQUIRE(result == cases.expected);
}

TEST_CASE("CheckMouseOverUIElement toggles Panel Element", "[collision]") {

  // create Panel Element and set position and size
  steamrot::PanelElement panel_element;
  panel_element.position = {0, 0};
  panel_element.size = {100, 100};

  // ensure is_mouse_over is false initially
  REQUIRE(panel_element.is_mouse_over == false);
  // check mouse position inside bounds
  sf::Vector2i mouse_position(50, 50);
  steamrot::collision::CheckMouseOverUIElement(mouse_position, panel_element);
  REQUIRE(panel_element.is_mouse_over == true);
  // now move mouse outside bounds
  mouse_position = sf::Vector2i(150, 150);
  steamrot::collision::CheckMouseOverUIElement(mouse_position, panel_element);
  REQUIRE(panel_element.is_mouse_over == false);
}

TEST_CASE("CheckMouseOverNestedUIElement toggles parent and child elements",
          "[collision]") {
  // create parent Panel Element and set position and size
  steamrot::PanelElement parent_element;
  parent_element.position = {0, 0};
  parent_element.size = {200, 200};
  // create child Panel Element and set position and size
  auto child_element_to_add = std::make_unique<steamrot::PanelElement>();
  child_element_to_add->position = {50, 50};
  child_element_to_add->size = {100, 100};
  // add child to parent
  parent_element.child_elements.push_back(std::move(child_element_to_add));
  auto &child_element = *parent_element.child_elements[0].get();

  // ensure is_mouse_over is false initially for both
  REQUIRE(parent_element.is_mouse_over == false);
  REQUIRE(child_element.is_mouse_over == false);
  // check mouse position inside child bounds
  sf::Vector2i mouse_position(75, 75);
  steamrot::collision::CheckMouseOverNestedUIElement(mouse_position,
                                                     parent_element);
  REQUIRE(child_element.is_mouse_over == true);
  REQUIRE(parent_element.is_mouse_over == false);
  // now move mouse outside child but inside parent bounds
  mouse_position = sf::Vector2i(25, 25);
  steamrot::collision::CheckMouseOverNestedUIElement(mouse_position,
                                                     parent_element);
  REQUIRE(child_element.is_mouse_over == false);
  REQUIRE(parent_element.is_mouse_over == true);
  // now move mouse outside both bounds
  mouse_position = sf::Vector2i(250, 250);
  steamrot::collision::CheckMouseOverNestedUIElement(mouse_position,
                                                     parent_element);
  REQUIRE(child_element.is_mouse_over == false);
  REQUIRE(parent_element.is_mouse_over == false);
}
