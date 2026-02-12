/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for collision handling of Grimoire Machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_grimoire_machina.h"
#include "MachinaFormScaffold.h"
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE(
    "CheckMouseOverGrowthPoint changes bool when mouse is over growth point",
    "[collision][grimoire_machina]") {

  SECTION("Mouse is over growth point") {
    steamrot::GrowthPoint growth_point;
    growth_point.origin.setPosition({100.f, 100.f});
    sf::Vector2i mouse_position(110.f, 110.f);
    steamrot::logic::collision::grimoire_machina::CheckMouseOverGrowthPoint(
        mouse_position, growth_point);
    REQUIRE(growth_point.is_mouse_over == true);
  }
  SECTION("Mouse is not over growth point") {
    steamrot::GrowthPoint growth_point;
    growth_point.origin.setPosition({100.f, 100.f});
    sf::Vector2i mouse_position(200.f, 200.f);
    steamrot::logic::collision::grimoire_machina::CheckMouseOverGrowthPoint(
        mouse_position, growth_point);
    REQUIRE(growth_point.is_mouse_over == false);
  }
}
