/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for positioning_ghost free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_ghost.h"
#include "Fragment.h"
#include "MrGhost.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <catch2/catch_test_macros.hpp>

namespace {

/////////////////////////////////////////////////
steamrot::Fragment MakePopulatedFragment() {
  steamrot::Fragment fragment;
  sf::VertexArray view(sf::PrimitiveType::Triangles, 6);
  view[0].position = {0.f, 0.f};
  view[1].position = {20.f, 0.f};
  view[2].position = {20.f, 20.f};
  view[3].position = {0.f, 0.f};
  view[4].position = {20.f, 20.f};
  view[5].position = {0.f, 20.f};
  fragment.positioning_views.insert_or_assign(steamrot::ViewDirection::Front,
                                              view);
  return fragment;
}

} // anonymous namespace

/////////////////////////////////////////////////
// RotateGhost
/////////////////////////////////////////////////

TEST_CASE("RotateGhost increments rotation by 90 degrees",
          "[unit][positioning_ghost]") {
  steamrot::MrGhost mr_ghost;
  REQUIRE(mr_ghost.m_rotation_degrees == 0.f);

  steamrot::logic::positioning::ghost::RotateGhost(mr_ghost);
  REQUIRE(mr_ghost.m_rotation_degrees == 90.f);
}

TEST_CASE("RotateGhost accumulates rotation across multiple calls",
          "[unit][positioning_ghost]") {
  steamrot::MrGhost mr_ghost;

  steamrot::logic::positioning::ghost::RotateGhost(mr_ghost);
  REQUIRE(mr_ghost.m_rotation_degrees == 90.f);

  steamrot::logic::positioning::ghost::RotateGhost(mr_ghost);
  REQUIRE(mr_ghost.m_rotation_degrees == 180.f);

  steamrot::logic::positioning::ghost::RotateGhost(mr_ghost);
  REQUIRE(mr_ghost.m_rotation_degrees == 270.f);
}

TEST_CASE("RotateGhost wraps rotation back to 0 after four calls",
          "[unit][positioning_ghost]") {
  steamrot::MrGhost mr_ghost;

  steamrot::logic::positioning::ghost::RotateGhost(mr_ghost);
  steamrot::logic::positioning::ghost::RotateGhost(mr_ghost);
  steamrot::logic::positioning::ghost::RotateGhost(mr_ghost);
  steamrot::logic::positioning::ghost::RotateGhost(mr_ghost);

  REQUIRE(mr_ghost.m_rotation_degrees == 0.f);
}

TEST_CASE("UpdatePosition copies the authoritative world mouse position",
          "[unit][positioning_ghost]") {
  steamrot::MrGhost mr_ghost;

  steamrot::logic::positioning::ghost::UpdatePosition(mr_ghost, {50.f, 75.f});

  REQUIRE(mr_ghost.m_position.x == 50.f);
  REQUIRE(mr_ghost.m_position.y == 75.f);
}

TEST_CASE("UpdatePosition rebuilds the active ghost transform from world mouse "
          "position",
          "[unit][positioning_ghost]") {
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_rotation_degrees = 90.f;
  mr_ghost.m_instance.emplace<steamrot::FragmentInstance>(0, MakePopulatedFragment());

  steamrot::logic::positioning::ghost::UpdatePosition(mr_ghost, {80.f, 90.f});

  const auto &instance =
      std::get<steamrot::FragmentInstance>(mr_ghost.m_instance);
  const sf::Vector2f transformed_center =
      instance.getTransform().transformPoint({10.f, 10.f});

  REQUIRE(mr_ghost.m_position.x == 80.f);
  REQUIRE(mr_ghost.m_position.y == 90.f);
  REQUIRE(transformed_center.x == 75.f);
  REQUIRE(transformed_center.y == 85.f);
}
