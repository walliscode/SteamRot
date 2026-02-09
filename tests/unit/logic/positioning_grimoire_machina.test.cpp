/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for positioning grimoire machina free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include "PanelElement.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CalculateCraftingCanvasSizeAndPosition returns a rectangle shape "
          "with the correct size and position",
          "[positioning][grimoire_machina]") {
  // Arrange
  sf::RectangleShape texture_coordinates(sf::Vector2f(1000.0f, 1000.0f));
  texture_coordinates.setPosition(sf::Vector2f(0.0f, 0.0f));

  std::vector<steamrot::CUserInterface> crafting_ui_elements;

  SECTION("When there are no crafting UI elements") {
    // Act
    sf::RectangleShape crafting_canvas = steamrot::logic::positioning::
        grimoire_machina::CalculateCraftingCanvasSizeAndPosition(
            texture_coordinates, crafting_ui_elements);
    // Assert
    REQUIRE(crafting_canvas.getSize() == sf::Vector2f(1000.0f, 1000.0f));
    REQUIRE(crafting_canvas.getPosition() == sf::Vector2f(0.0f, 0.0f));
  }

  SECTION("Crafting canvas x size and position are reduced by the size of the "
          "crafting UI elements") {
    // Arrange
    steamrot::CUserInterface ui_element;

    ui_element.m_name = "Crafting UI Element";
    std::unique_ptr<steamrot::UIElement> root_element =
        std::make_unique<steamrot::PanelElement>();
    root_element->size = sf::Vector2f(200.0f, 0.0f);
    root_element->position = sf::Vector2f(0.0f, 0.0f);
    ui_element.m_root_element = std::move(root_element);
    crafting_ui_elements.push_back(ui_element);
    // Act
    sf::RectangleShape crafting_canvas = steamrot::logic::positioning::
        grimoire_machina::CalculateCraftingCanvasSizeAndPosition(
            texture_coordinates, crafting_ui_elements);
    // Assert
    REQUIRE(crafting_canvas.getSize() == sf::Vector2f(800.0f, 1000.0f));
    REQUIRE(crafting_canvas.getPosition() == sf::Vector2f(200.0f, 0.0f));
  }
}
