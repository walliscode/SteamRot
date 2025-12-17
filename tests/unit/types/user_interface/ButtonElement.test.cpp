/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for BUttonElement type
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ButtonElement.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ButtonElement: Default Constructor", "[types]") {
  steamrot::ButtonElement button;

  REQUIRE(button.label == "unlabelled");
  REQUIRE(button.position == sf::Vector2f{0.f, 0.f});
  REQUIRE(button.size == sf::Vector2f{0.f, 0.f});
  REQUIRE(button.is_mouse_over == false);
  REQUIRE(button.subscription == nullptr);
  REQUIRE(button.response_event == std::nullopt);
  REQUIRE(button.children_active == false);
  REQUIRE(button.child_elements.empty());
  REQUIRE(button.layout == steamrot::Layout::Vertical);
  REQUIRE(button.spacing_strategy == steamrot::SpacingAndSizing::Even);
}

TEST_CASE("ButtonElement: Clone Method", "[types]") {
  steamrot::ButtonElement original;
  original.label = "Click Me!";
  original.position = sf::Vector2f{10.f, 20.f};
  original.size = sf::Vector2f{100.f, 50.f};
  original.is_mouse_over = true;
  original.children_active = true;
  original.layout = steamrot::Layout::Horizontal;
  original.spacing_strategy = steamrot::SpacingAndSizing::Even;
  auto cloned_ptr = original.Clone();
  auto *cloned = dynamic_cast<steamrot::ButtonElement *>(cloned_ptr.get());
  REQUIRE(cloned != nullptr);
  REQUIRE(cloned->label == "Click Me!");
  REQUIRE(cloned->position == sf::Vector2f{10.f, 20.f});
  REQUIRE(cloned->size == sf::Vector2f{100.f, 50.f});
  REQUIRE(cloned->is_mouse_over == true);
  REQUIRE(cloned->children_active == true);
  REQUIRE(cloned->layout == steamrot::Layout::Horizontal);
  REQUIRE(cloned->spacing_strategy == steamrot::SpacingAndSizing::Even);
}
