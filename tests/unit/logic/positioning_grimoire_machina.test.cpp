/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for positioning grimoire machina free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include "MachinaFormScaffold.h"
#include "PanelElement.h"
#include "Vector2fEqualsMatcher.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

TEST_CASE("CalculateCraftingCanvasSizeAndPosition returns a rectangle shape "
          "with the correct size and position",
          "[positioning][grimoire_machina]") {
  // Arrange
  sf::FloatRect texture_coordinates(sf::Vector2f(0.0f, 0.0f),
                                    sf::Vector2f(1000.0f, 1000.0f));

  std::vector<steamrot::CUserInterface> crafting_ui_elements;

  SECTION("When there are no crafting UI elements") {
    // Act
    sf::FloatRect crafting_canvas = steamrot::logic::positioning::
        grimoire_machina::CalculateCraftingCanvasSizeAndPosition(
            texture_coordinates, crafting_ui_elements);
    // Assert
    REQUIRE(crafting_canvas.size == sf::Vector2f(1000.0f, 1000.0f));
    REQUIRE(crafting_canvas.position == sf::Vector2f(0.0f, 0.0f));
  }
  SECTION("When the UI element is not visible") {
    // Arrange
    steamrot::CUserInterface ui_element;
    ui_element.m_visible = false;
    ui_element.m_name = "Crafting UI Element";
    std::unique_ptr<steamrot::UIElement> root_element =
        std::make_unique<steamrot::PanelElement>();
    root_element->size = sf::Vector2f(200.0f, 100.0f);
    root_element->position = sf::Vector2f(0.0f, 0.0f);
    ui_element.m_root_element = std::move(root_element);
    crafting_ui_elements.push_back(ui_element);
    // Act
    sf::FloatRect crafting_canvas = steamrot::logic::positioning::
        grimoire_machina::CalculateCraftingCanvasSizeAndPosition(
            texture_coordinates, crafting_ui_elements);
    // Assert
    REQUIRE(crafting_canvas.size == sf::Vector2f(1000.0f, 1000.0f));
    REQUIRE(crafting_canvas.position == sf::Vector2f(0.0f, 0.0f));
  }

  SECTION("Crafting canvas x size and position are reduced by the size of the "
          "crafting UI elements") {
    // Arrange
    steamrot::CUserInterface ui_element;

    ui_element.m_visible = true;
    ui_element.m_name = "Crafting UI Element";
    std::unique_ptr<steamrot::UIElement> root_element =
        std::make_unique<steamrot::PanelElement>();
    root_element->size = sf::Vector2f(200.0f, 100.0f);
    root_element->position = sf::Vector2f(0.0f, 0.0f);
    ui_element.m_root_element = std::move(root_element);
    crafting_ui_elements.push_back(ui_element);

    sf::FloatRect expected_crafting_canvas(sf::Vector2f(200.0f, 0.0f),
                                           sf::Vector2f(800.0f, 1000.0f));
    // Act
    sf::FloatRect actual_crafting_canvas = steamrot::logic::positioning::
        grimoire_machina::CalculateCraftingCanvasSizeAndPosition(
            texture_coordinates, crafting_ui_elements);

    // Assert
    REQUIRE_THAT(
        actual_crafting_canvas.size,
        steamrot::tests::EqualsVector2f(expected_crafting_canvas.size));
    REQUIRE_THAT(
        actual_crafting_canvas.position,
        steamrot::tests::EqualsVector2f(expected_crafting_canvas.position));
  }
  SECTION(
      "Crafting canvas x size and position are not reduced by the size of the "
      "crafting UI elements if they do not overlap") {
    // Arrange
    steamrot::CUserInterface ui_element;
    ui_element.m_visible = true;
    ui_element.m_name = "Crafting UI Element";
    std::unique_ptr<steamrot::UIElement> root_element =
        std::make_unique<steamrot::PanelElement>();
    root_element->size = sf::Vector2f(200.0f, 100.f);
    root_element->position = sf::Vector2f(1001.0f, 1001.0f);
    ui_element.m_root_element = std::move(root_element);
    crafting_ui_elements.push_back(ui_element);
    sf::FloatRect expected_crafting_canvas(sf::Vector2f(0.0f, 0.0f),
                                           sf::Vector2f(1000.0f, 1000.0f));
    // Act
    sf::FloatRect actual_crafting_canvas = steamrot::logic::positioning::
        grimoire_machina::CalculateCraftingCanvasSizeAndPosition(
            texture_coordinates, crafting_ui_elements);
    // Assert
    REQUIRE_THAT(
        actual_crafting_canvas.size,
        steamrot::tests::EqualsVector2f(expected_crafting_canvas.size));
    REQUIRE_THAT(
        actual_crafting_canvas.position,
        steamrot::tests::EqualsVector2f(expected_crafting_canvas.position));
  }

  SECTION("Crafting canvas is reduced by multiple crafting UI elements") {
    // Arrange
    steamrot::CUserInterface ui_element1;
    ui_element1.m_visible = true;
    ui_element1.m_name = "Crafting UI Element 1";
    std::unique_ptr<steamrot::UIElement> root_element1 =
        std::make_unique<steamrot::PanelElement>();
    root_element1->size = sf::Vector2f(200.0f, 100.0f);
    root_element1->position = sf::Vector2f(0.0f, 0.0f);
    ui_element1.m_root_element = std::move(root_element1);
    crafting_ui_elements.push_back(ui_element1);
    steamrot::CUserInterface ui_element2;
    ui_element2.m_visible = true;
    ui_element2.m_name = "Crafting UI Element 2";
    std::unique_ptr<steamrot::UIElement> root_element2 =
        std::make_unique<steamrot::PanelElement>();
    root_element2->size = sf::Vector2f(100.0f, 78.0f);
    root_element2->position = sf::Vector2f(200.0f, 0.0f);
    ui_element2.m_root_element = std::move(root_element2);
    crafting_ui_elements.push_back(ui_element2);
    sf::FloatRect expected_crafting_canvas(sf::Vector2f(300.0f, 0.0f),
                                           sf::Vector2f(700.0f, 1000.0f));
    // Act
    sf::FloatRect actual_crafting_canvas = steamrot::logic::positioning::
        grimoire_machina::CalculateCraftingCanvasSizeAndPosition(
            texture_coordinates, crafting_ui_elements);
    // Assert
    REQUIRE_THAT(
        actual_crafting_canvas.size,
        steamrot::tests::EqualsVector2f(expected_crafting_canvas.size));
    REQUIRE_THAT(
        actual_crafting_canvas.position,
        steamrot::tests::EqualsVector2f(expected_crafting_canvas.position));
  }
}

TEST_CASE("PositionGirowthPoint positions the growth point in the center of "
          "the crafting canvas",
          "[positioning][grimoire_machina]") {

  // Arrange
  steamrot::GrowthPoint growth_point;
  REQUIRE(growth_point.origin.getPosition() == sf::Vector2f(0.0f, 0.0f));

  // Act
  std::vector<sf::FloatRect> crafting_canvas_cases = {
      sf::FloatRect(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(1000.0f, 1000.0f)),
      sf::FloatRect(sf::Vector2f(200.0f, 100.0f), sf::Vector2f(800.0f, 900.0f)),
      sf::FloatRect(sf::Vector2f(300.0f, 200.0f),
                    sf::Vector2f(700.0f, 800.0f))};

  // Assert
  for (const sf::FloatRect &crafting_canvas : crafting_canvas_cases) {
    steamrot::logic::positioning::grimoire_machina::PositionGrowthPoint(
        growth_point, crafting_canvas);
    sf::Vector2f expected_position =
        crafting_canvas.position + crafting_canvas.size / 2.f;
    REQUIRE_THAT(growth_point.origin.getPosition(),
                 steamrot::tests::EqualsVector2f(expected_position));
  }
}
