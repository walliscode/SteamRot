/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the movement_camera free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "movement_camera.h"
#include "CameraState.h"
#include <catch2/catch_test_macros.hpp>
#include <cmath>

/////////////////////////////////////////////////
// ApplyZoom
/////////////////////////////////////////////////

TEST_CASE("movement_camera::ApplyZoom: positive delta decreases zoom level",
          "[unit][movement_camera]") {
  steamrot::CameraState camera_state;
  const float initial_zoom = camera_state.m_zoom_level;

  steamrot::logic::movement::camera::ApplyZoom(camera_state, 1.0f);

  REQUIRE(camera_state.m_zoom_level < initial_zoom);
}

TEST_CASE("movement_camera::ApplyZoom: negative delta increases zoom level",
          "[unit][movement_camera]") {
  steamrot::CameraState camera_state;
  const float initial_zoom = camera_state.m_zoom_level;

  steamrot::logic::movement::camera::ApplyZoom(camera_state, -1.0f);

  REQUIRE(camera_state.m_zoom_level > initial_zoom);
}

TEST_CASE("movement_camera::ApplyZoom: zero delta leaves zoom level unchanged",
          "[unit][movement_camera]") {
  steamrot::CameraState camera_state;
  const float initial_zoom = camera_state.m_zoom_level;

  steamrot::logic::movement::camera::ApplyZoom(camera_state, 0.0f);

  REQUIRE(camera_state.m_zoom_level == initial_zoom);
}

TEST_CASE("movement_camera::ApplyZoom: clamps zoom to kMinZoom",
          "[unit][movement_camera]") {
  steamrot::CameraState camera_state;

  // Large positive delta should zoom in past the minimum
  steamrot::logic::movement::camera::ApplyZoom(camera_state, 1000.0f);

  REQUIRE(camera_state.m_zoom_level == steamrot::CameraState::kMinZoom);
}

TEST_CASE("movement_camera::ApplyZoom: clamps zoom to kMaxZoom",
          "[unit][movement_camera]") {
  steamrot::CameraState camera_state;

  // Large negative delta should zoom out past the maximum
  steamrot::logic::movement::camera::ApplyZoom(camera_state, -1000.0f);

  REQUIRE(camera_state.m_zoom_level == steamrot::CameraState::kMaxZoom);
}

/////////////////////////////////////////////////
// GetWorldView / MapToWorldCoords
/////////////////////////////////////////////////

TEST_CASE(
    "movement_camera::GetWorldView: default camera centres view on origin",
    "[unit][movement_camera]") {
  steamrot::CameraState camera_state;
  sf::RenderTexture texture;
  texture.resize({800u, 600u});

  const sf::View view =
      steamrot::logic::movement::camera::GetWorldView(camera_state, texture);

  REQUIRE(view.getCenter().x == 0.f);
  REQUIRE(view.getCenter().y == 0.f);
}

TEST_CASE(
    "movement_camera::GetWorldView: non-default position shifts view centre",
    "[unit][movement_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_position = {100.f, 200.f};
  sf::RenderTexture texture;
  texture.resize({800u, 600u});

  const sf::View view =
      steamrot::logic::movement::camera::GetWorldView(camera_state, texture);

  REQUIRE(view.getCenter().x == 100.f);
  REQUIRE(view.getCenter().y == 200.f);
}

TEST_CASE(
    "movement_camera::MapToWorldCoords: default camera maps screen centre to "
    "world origin",
    "[unit][movement_camera]") {
  steamrot::CameraState camera_state;
  sf::RenderTexture texture;
  constexpr unsigned int w = 800u;
  constexpr unsigned int h = 600u;
  texture.resize({w, h});

  const sf::Vector2f world_pos =
      steamrot::logic::movement::camera::MapToWorldCoords(
          camera_state, {static_cast<int>(w / 2), static_cast<int>(h / 2)},
          texture);

  // Screen centre should map to world (0, 0) when camera is at origin
  REQUIRE(std::abs(world_pos.x) < 0.01f);
  REQUIRE(std::abs(world_pos.y) < 0.01f);
}
