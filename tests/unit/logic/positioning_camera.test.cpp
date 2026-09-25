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

TEST_CASE("positioning_camera::GetUIView returns the texture default view",
          "[unit][positioning_camera]") {
  sf::RenderTexture texture;
  auto resize_result = texture.resize({640u, 480u});
  if (!resize_result) {
    FAIL("Failed to resize RenderTexture");
  }

  const sf::View default_view = texture.getDefaultView();
  const sf::View ui_view =
      steamrot::logic::positioning::camera::get_ui_view(texture);

  REQUIRE(ui_view.getCenter() == default_view.getCenter());
  REQUIRE(ui_view.getSize() == default_view.getSize());
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

TEST_CASE("positioning_camera::update_world_mouse_position uses finalized camera "
          "state",
          "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_panning_right = true;
  camera_state.m_panning_down = true;

  sf::RenderTexture texture;
  constexpr unsigned int w = 800u;
  constexpr unsigned int h = 600u;
  auto resize_result = texture.resize({w, h});
  if (!resize_result) {
    FAIL("Failed to resize RenderTexture");
  }

  steamrot::logic::positioning::camera::apply_pan(camera_state);

  sf::Vector2f world_mouse_position{-999.f, -999.f};
  steamrot::logic::positioning::camera::update_world_mouse_position(
      world_mouse_position, camera_state,
      {static_cast<int>(w / 2), static_cast<int>(h / 2)}, texture);

  REQUIRE(world_mouse_position.x == camera_state.m_position.x);
  REQUIRE(world_mouse_position.y == camera_state.m_position.y);
}

TEST_CASE("positioning_camera::apply_world_view and apply_ui_view set explicit "
          "render views",
          "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_position = {120.f, 80.f};
  camera_state.m_zoom_level = 2.f;

  sf::RenderTexture texture;
  auto resize_result = texture.resize({800u, 600u});
  if (!resize_result) {
    FAIL("Failed to resize RenderTexture");
  }

  steamrot::logic::positioning::camera::apply_world_view(texture, camera_state);
  REQUIRE(texture.getView().getCenter() ==
          steamrot::logic::positioning::camera::get_world_view(camera_state,
                                                                texture)
              .getCenter());

  steamrot::logic::positioning::camera::apply_ui_view(texture);
  REQUIRE(texture.getView().getCenter().x ==
          texture.getDefaultView().getCenter().x);
  REQUIRE(texture.getView().getCenter().y ==
          texture.getDefaultView().getCenter().y);
  REQUIRE(texture.getView().getSize().x == texture.getDefaultView().getSize().x);
  REQUIRE(texture.getView().getSize().y == texture.getDefaultView().getSize().y);
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
