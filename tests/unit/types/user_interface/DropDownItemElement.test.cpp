/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for DropDownItemElement type
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DropDownItemElement.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("DropDownItemElement: Default Constructor", "[types]") {
  steamrot::DropDownItemElement item;

  REQUIRE(item.position == sf::Vector2f{0.f, 0.f});
  REQUIRE(item.size == sf::Vector2f{0.f, 0.f});
  REQUIRE(item.is_mouse_over == false);
  REQUIRE(item.subscription == nullptr);
  REQUIRE(item.response_events.empty());
  REQUIRE(item.children_active == false);
  REQUIRE(item.child_elements.empty());
  REQUIRE(item.layout == steamrot::Layout::Vertical);
  REQUIRE(item.spacing_strategy == steamrot::SpacingAndSizing::Even);
  REQUIRE(item.label == "item...");
  REQUIRE(item.value == "value...");
}

TEST_CASE("DropDownItemElement: Clone Method", "[types]") {
  steamrot::DropDownItemElement original;
  original.position = sf::Vector2f{10.f, 20.f};
  original.size = sf::Vector2f{100.f, 50.f};
  original.is_mouse_over = true;
  original.children_active = true;
  original.layout = steamrot::Layout::Horizontal;
  original.spacing_strategy = steamrot::SpacingAndSizing::Even;
  original.label = "Option 1";
  original.value = "opt_1";

  auto cloned_ptr = original.Clone();
  auto *cloned = dynamic_cast<steamrot::DropDownItemElement *>(cloned_ptr.get());

  REQUIRE(cloned != nullptr);
  REQUIRE(cloned->position == sf::Vector2f{10.f, 20.f});
  REQUIRE(cloned->size == sf::Vector2f{100.f, 50.f});
  REQUIRE(cloned->is_mouse_over == true);
  REQUIRE(cloned->children_active == true);
  REQUIRE(cloned->layout == steamrot::Layout::Horizontal);
  REQUIRE(cloned->spacing_strategy == steamrot::SpacingAndSizing::Even);
  REQUIRE(cloned->label == "Option 1");
  REQUIRE(cloned->value == "opt_1");
}
