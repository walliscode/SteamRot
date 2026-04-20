/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for positioning_ghost free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_ghost.h"
#include "MrGhost.h"
#include <catch2/catch_test_macros.hpp>

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
