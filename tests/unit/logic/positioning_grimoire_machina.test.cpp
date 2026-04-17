/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for positioning grimoire machina free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include "Fragment.h"
#include "MachinaFormScaffold.h"
#include "Vector2fEqualsMatcher.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
/// @brief Helper: build a Views object with a single coloured triangle in the
/// Front slot.
/////////////////////////////////////////////////
steamrot::Views MakeViewsWithFrontTriangle(sf::Color colour = sf::Color::Red) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{10.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{20.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{15.f, 20.f}, colour});

  steamrot::Views views;
  views.insert_or_assign(steamrot::ViewDirection::Front, std::move(va));
  return views;
}

/////////////////////////////////////////////////
/// @brief Helper: build a Fragment with a coloured triangle in the Front slot.
/////////////////////////////////////////////////
steamrot::Fragment
MakeFragmentWithFrontView(sf::Color colour = sf::Color::Green) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{5.f, 5.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{25.f, 5.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{15.f, 25.f}, colour});

  steamrot::Fragment fragment;
  fragment.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                           std::move(va));
  return fragment;
}
TEST_CASE("position_first_part_of_machina_form tests",
          "[positioning_grimoire_machina]") {
  // Arrange
  steamrot::PartMap parts;

  SECTION("Does not throw when parts is empty") {
    // Act & Assert
    REQUIRE_NOTHROW(steamrot::logic::positioning::grimoire_machina::
                        position_first_part_of_machina_form_scaffold(parts));
  }

  SECTION("Does not throw when Fragment has no sockets") {
    // Arrange
    steamrot::Fragment fragment{}; // empty fragment
    steamrot::FragmentInstance fragment_instance{
        fragment}; // instance of that fragment
    fragment_instance.id = 0;

    parts.emplace(fragment_instance.id, fragment_instance); // add to parts map

    REQUIRE(parts.size() == 1); // sanity check
    // Act & Assert
    REQUIRE_NOTHROW(steamrot::logic::positioning::grimoire_machina::
                        position_first_part_of_machina_form_scaffold(parts));
  }

  SECTION("Positions centre of first PartInstance's FRONT view at 0,0") {
    // Arrange
    auto fragment = MakeFragmentWithFrontView();
    steamrot::FragmentInstance fragment_instance{fragment};
    fragment_instance.id = 0;
    parts.emplace(fragment_instance.id, fragment_instance);
    REQUIRE(parts.size() == 1); // sanity check
    // pull out reference to the Fragmentinstance we just added so we can check
    // its transform after
    steamrot::FragmentInstance &instance =
        std::get<steamrot::FragmentInstance>(parts.at(0));

    // Act
    steamrot::logic::positioning::grimoire_machina::
        position_first_part_of_machina_form_scaffold(parts);
    // Assert: the triangle's centroid is at (15, 11.67) local → should be at
    // (0,0) world
    sf::Vector2f expected_position{0.f, 0.f};
    sf::Vector2f actual_position =
        instance.transform.transformPoint({15.f, 11.67f});

    REQUIRE_THAT(actual_position,
                 steamrot::tests::EqualsVector2f(expected_position));
  }
}
