/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the positioning_camera free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_camera.h"
#include "CameraState.h"
#include <catch2/catch_test_macros.hpp>
#include <cmath>

/////////////////////////////////////////////////
// GetWorldView / MapToWorldCoords
/////////////////////////////////////////////////

TEST_CASE(
    "positioning_camera::GetWorldView: default camera centres view on origin",
    "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  sf::RenderTexture texture;
  auto resize_result = texture.resize({800u, 600u});
  if (!resize_result) {
    FAIL("Failed to resize RenderTexture");
  }

  const sf::View view = steamrot::logic::positioning::camera::get_world_view(
      camera_state, texture);

  REQUIRE(view.getCenter().x == 0.f);
  REQUIRE(view.getCenter().y == 0.f);
}

TEST_CASE(
    "positioning_camera::GetWorldView: non-default position shifts view centre",
    "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_position = {100.f, 200.f};
  sf::RenderTexture texture;

  auto resize_result = texture.resize({800u, 600u});
  if (!resize_result) {
    FAIL("Failed to resize RenderTexture");
  }

  const sf::View view = steamrot::logic::positioning::camera::get_world_view(
      camera_state, texture);

  REQUIRE(view.getCenter().x == 100.f);
  REQUIRE(view.getCenter().y == 200.f);
}

TEST_CASE("positioning_camera::MapToWorldCoords: default camera maps screen "
          "centre to "
          "world origin",
          "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  sf::RenderTexture texture;
  constexpr unsigned int w = 800u;
  constexpr unsigned int h = 600u;

  auto resize_result = texture.resize({800u, 600u});
  if (!resize_result) {
    FAIL("Failed to resize RenderTexture");
  }

  const sf::Vector2f world_pos =
      steamrot::logic::positioning::camera::map_to_world_coords(
          camera_state, {static_cast<int>(w / 2), static_cast<int>(h / 2)},
          texture);

  // Screen centre should map to world (0, 0) when camera is at origin
  REQUIRE(std::abs(world_pos.x) < 0.01f);
  REQUIRE(std::abs(world_pos.y) < 0.01f);
}

/////////////////////////////////////////////////
// ApplyPan
/////////////////////////////////////////////////

TEST_CASE("positioning_camera::apply_pan: no pan flags active leaves position "
          "unchanged",
          "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  steamrot::logic::positioning::camera::apply_pan(camera_state);
  REQUIRE(camera_state.m_position.x == 0.f);
  REQUIRE(camera_state.m_position.y == 0.f);
}

TEST_CASE(
    "positioning_camera::apply_pan: panning_right moves position x by kPanSpeed",
    "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_panning_right = true;
  steamrot::logic::positioning::camera::apply_pan(camera_state);
  REQUIRE(camera_state.m_position.x == steamrot::CameraState::kPanSpeed);
  REQUIRE(camera_state.m_position.y == 0.f);
}

TEST_CASE(
    "positioning_camera::apply_pan: panning_left moves position x by -kPanSpeed",
    "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_panning_left = true;
  steamrot::logic::positioning::camera::apply_pan(camera_state);
  REQUIRE(camera_state.m_position.x == -steamrot::CameraState::kPanSpeed);
  REQUIRE(camera_state.m_position.y == 0.f);
}

TEST_CASE(
    "positioning_camera::apply_pan: panning_down moves position y by kPanSpeed",
    "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_panning_down = true;
  steamrot::logic::positioning::camera::apply_pan(camera_state);
  REQUIRE(camera_state.m_position.x == 0.f);
  REQUIRE(camera_state.m_position.y == steamrot::CameraState::kPanSpeed);
}

TEST_CASE(
    "positioning_camera::apply_pan: panning_up moves position y by -kPanSpeed",
    "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_panning_up = true;
  steamrot::logic::positioning::camera::apply_pan(camera_state);
  REQUIRE(camera_state.m_position.x == 0.f);
  REQUIRE(camera_state.m_position.y == -steamrot::CameraState::kPanSpeed);
}

TEST_CASE("positioning_camera::apply_pan: simultaneous panning_right and "
          "panning_down produces diagonal movement",
          "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_panning_right = true;
  camera_state.m_panning_down = true;
  steamrot::logic::positioning::camera::apply_pan(camera_state);
  REQUIRE(camera_state.m_position.x == steamrot::CameraState::kPanSpeed);
  REQUIRE(camera_state.m_position.y == steamrot::CameraState::kPanSpeed);
}

TEST_CASE("positioning_camera::apply_pan: opposing panning flags cancel out",
          "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_panning_left = true;
  camera_state.m_panning_right = true;
  steamrot::logic::positioning::camera::apply_pan(camera_state);
  REQUIRE(camera_state.m_position.x == 0.f);
}

TEST_CASE(
    "positioning_camera::apply_pan: accumulates over multiple calls",
    "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_panning_right = true;
  steamrot::logic::positioning::camera::apply_pan(camera_state);
  steamrot::logic::positioning::camera::apply_pan(camera_state);
  REQUIRE(camera_state.m_position.x ==
          2.f * steamrot::CameraState::kPanSpeed);
}
