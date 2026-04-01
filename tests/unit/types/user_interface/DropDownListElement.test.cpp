/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for DropDownListElement type
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DropDownListElement.h"
#include "DataPopulationFunctions.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("DropDownListElement: Default Constructor", "[types]") {
  steamrot::DropDownListElement dropdown;

  REQUIRE(dropdown.position == sf::Vector2f{0.f, 0.f});
  REQUIRE(dropdown.size == sf::Vector2f{0.f, 0.f});
  REQUIRE(dropdown.is_mouse_over == false);
  REQUIRE(dropdown.subscription == nullptr);
  REQUIRE(dropdown.response_events.empty());
  REQUIRE(dropdown.children_active == false);
  REQUIRE(dropdown.child_elements.empty());
  REQUIRE(dropdown.layout == steamrot::Layout::Vertical);
  REQUIRE(dropdown.spacing_strategy == steamrot::SpacingAndSizing::Even);
  REQUIRE(dropdown.is_expanded == false);
  REQUIRE(dropdown.unexpanded_label == "unexpanded items...");
  REQUIRE(dropdown.expanded_label == "expanded items...");
  REQUIRE(dropdown.data_population_function ==
          steamrot::DataPopulationFunction::None);
}

TEST_CASE("DropDownListElement: Clone Method", "[types]") {
  steamrot::DropDownListElement original;
  original.position = sf::Vector2f{10.f, 20.f};
  original.size = sf::Vector2f{100.f, 50.f};
  original.is_mouse_over = true;
  original.children_active = true;
  original.layout = steamrot::Layout::Horizontal;
  original.spacing_strategy = steamrot::SpacingAndSizing::Even;
  original.is_expanded = true;
  original.unexpanded_label = "Select item";
  original.expanded_label = "Hide items";
  original.data_population_function =
      steamrot::DataPopulationFunction::GetAllFragmentNames;

  auto cloned_ptr = original.Clone();
  auto *cloned =
      dynamic_cast<steamrot::DropDownListElement *>(cloned_ptr.get());

  REQUIRE(cloned != nullptr);
  REQUIRE(cloned->position == sf::Vector2f{10.f, 20.f});
  REQUIRE(cloned->size == sf::Vector2f{100.f, 50.f});
  REQUIRE(cloned->is_mouse_over == true);
  REQUIRE(cloned->children_active == true);
  REQUIRE(cloned->layout == steamrot::Layout::Horizontal);
  REQUIRE(cloned->spacing_strategy == steamrot::SpacingAndSizing::Even);
  REQUIRE(cloned->is_expanded == true);
  REQUIRE(cloned->unexpanded_label == "Select item");
  REQUIRE(cloned->expanded_label == "Hide items");
  REQUIRE(cloned->data_population_function ==
          steamrot::DataPopulationFunction::GetAllFragmentNames);
}
