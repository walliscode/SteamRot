/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for DropDownContainerElement type
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DropDownContainerElement.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("DropDownContainerElement: Default Constructor", "[types]") {
  steamrot::DropDownContainerElement container;

  REQUIRE(container.position == sf::Vector2f{0.f, 0.f});
  REQUIRE(container.size == sf::Vector2f{0.f, 0.f});
  REQUIRE(container.is_mouse_over == false);
  REQUIRE(container.subscription == nullptr);
  REQUIRE(container.response_events.empty());
  REQUIRE(container.children_active == false);
  REQUIRE(container.child_elements.empty());
  REQUIRE(container.is_expanded == false);
}

TEST_CASE("DropDownContainerElement: Clone Method", "[types]") {
  steamrot::DropDownContainerElement original;
  original.position = sf::Vector2f{10.f, 20.f};
  original.size = sf::Vector2f{100.f, 50.f};
  original.is_mouse_over = true;
  original.children_active = true;
  original.is_expanded = true;

  auto cloned_ptr = original.Clone();
  auto *cloned = dynamic_cast<steamrot::DropDownContainerElement *>(cloned_ptr.get());

  REQUIRE(cloned != nullptr);
  REQUIRE(cloned->position == sf::Vector2f{10.f, 20.f});
  REQUIRE(cloned->size == sf::Vector2f{100.f, 50.f});
  REQUIRE(cloned->is_mouse_over == true);
  REQUIRE(cloned->children_active == true);
  REQUIRE(cloned->is_expanded == true);
}
