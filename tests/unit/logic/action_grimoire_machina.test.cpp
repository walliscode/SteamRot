/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the GrimoireMachina action processing functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_grimoire_machina.h"
#include "MachinaFormScaffold.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("InitialiseActiveMachinaForm adds a new MachinaForm to the "
          "GrimoireMachina active form",

          "[GrimoireMachina]") {
  steamrot::GrimoireMachina grimoire_machina;

  auto result = steamrot::logic::actions::grimoire_machina::
      InitialiseActiveMachinaFormScaffold(grimoire_machina);
  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
}

TEST_CASE("ClearActiveMachinaForm clears the active MachinaForm in the "
          "GrimoireMachina",
          "[GrimoireMachina]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
  auto result = steamrot::logic::actions::grimoire_machina::
      ClearActiveMachinaFormScaffold(grimoire_machina);
  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
}

TEST_CASE(
    "SetGrowthPointColor sets the GrowthPoint origin color to hover_color "
    "when is_mouse_over is true",
    "[GrimoireMachina]") {
  steamrot::GrowthPoint growth_point;
  growth_point.is_mouse_over = true;
  growth_point.hover_color = sf::Color::Red;
  growth_point.base_color = sf::Color::Blue;
  steamrot::logic::actions::grimoire_machina::SetGrowthPointColor(growth_point);
  REQUIRE(growth_point.origin.getFillColor() == growth_point.hover_color);
}
TEST_CASE("SetGrowthPointColor sets the GrowthPoint origin color to base_color "
          "when is_mouse_over is false",
          "[GrimoireMachina]") {
  steamrot::GrowthPoint growth_point;
  growth_point.is_mouse_over = false;
  growth_point.hover_color = sf::Color::Red;
  growth_point.base_color = sf::Color::Blue;
  steamrot::logic::actions::grimoire_machina::SetGrowthPointColor(growth_point);
  REQUIRE(growth_point.origin.getFillColor() == growth_point.base_color);
}
