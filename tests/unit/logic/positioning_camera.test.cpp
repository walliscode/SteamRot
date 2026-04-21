/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the positioning_camera free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_camera.h"
#include "CameraState.h"
#include "PanelElement.h"
#include "SceneType.h"
#include "containers.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>
#include <cmath>

namespace {

/////////////////////////////////////////////////
/// Helper: build an EntityMemoryPool with @p size slots and activate the
/// CUserInterface at @p entity_id with a left-anchored panel of @p width.
/////////////////////////////////////////////////
steamrot::EntityMemoryPool
make_pool_with_left_toolbar(size_t size, size_t entity_id, float width) {
  steamrot::EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, size);

  auto &ui = steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
      entity_id, pool);
  ui.m_active = true;
  auto panel = std::make_unique<steamrot::PanelElement>();
  panel->position = {0.f, 0.f};
  panel->size = {width, 600.f};
  ui.m_root_element = std::move(panel);
  return pool;
}

} // namespace

/////////////////////////////////////////////////
// get_left_ui_toolbar_width
/////////////////////////////////////////////////

TEST_CASE("positioning_camera::get_left_ui_toolbar_width: empty pool returns 0",
          "[unit][positioning_camera]") {
  steamrot::EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 5);
  REQUIRE(steamrot::logic::positioning::camera::get_left_ui_toolbar_width(
              pool) == 0.f);
}

TEST_CASE("positioning_camera::get_left_ui_toolbar_width: inactive UI is "
          "ignored",
          "[unit][positioning_camera]") {
  steamrot::EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 5);

  auto &ui = steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
      0, pool);
  ui.m_active = false; // inactive — must not be counted
  auto panel = std::make_unique<steamrot::PanelElement>();
  panel->position = {0.f, 0.f};
  panel->size = {180.f, 600.f};
  ui.m_root_element = std::move(panel);

  REQUIRE(steamrot::logic::positioning::camera::get_left_ui_toolbar_width(
              pool) == 0.f);
}

TEST_CASE(
    "positioning_camera::get_left_ui_toolbar_width: non-left-anchored UI is "
    "ignored",
    "[unit][positioning_camera]") {
  steamrot::EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 5);

  auto &ui = steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
      0, pool);
  ui.m_active = true;
  auto panel = std::make_unique<steamrot::PanelElement>();
  panel->position = {200.f, 0.f}; // not at x=0
  panel->size = {180.f, 600.f};
  ui.m_root_element = std::move(panel);

  REQUIRE(steamrot::logic::positioning::camera::get_left_ui_toolbar_width(
              pool) == 0.f);
}

TEST_CASE("positioning_camera::get_left_ui_toolbar_width: single left toolbar "
          "returns its width",
          "[unit][positioning_camera]") {
  auto pool = make_pool_with_left_toolbar(5, 0, 180.f);
  REQUIRE(steamrot::logic::positioning::camera::get_left_ui_toolbar_width(
              pool) == 180.f);
}

TEST_CASE("positioning_camera::get_left_ui_toolbar_width: returns max width "
          "of multiple left-anchored panels",
          "[unit][positioning_camera]") {
  auto pool = make_pool_with_left_toolbar(5, 0, 180.f);

  // Add a second, wider left-anchored panel at entity 1
  auto &ui2 = steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
      1, pool);
  ui2.m_active = true;
  auto panel2 = std::make_unique<steamrot::PanelElement>();
  panel2->position = {0.f, 0.f};
  panel2->size = {200.f, 600.f};
  ui2.m_root_element = std::move(panel2);

  REQUIRE(steamrot::logic::positioning::camera::get_left_ui_toolbar_width(
              pool) == 200.f);
}

/////////////////////////////////////////////////
// get_scene_view_offset
/////////////////////////////////////////////////

TEST_CASE(
    "positioning_camera::get_scene_view_offset: TITLE returns zero offset",
    "[unit][positioning_camera]") {
  steamrot::EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 5);
  const sf::Vector2f offset =
      steamrot::logic::positioning::camera::get_scene_view_offset(
          steamrot::SceneType::TITLE, pool);
  REQUIRE(offset.x == 0.f);
  REQUIRE(offset.y == 0.f);
}

TEST_CASE("positioning_camera::get_scene_view_offset: TEST returns zero offset",
          "[unit][positioning_camera]") {
  steamrot::EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 5);
  const sf::Vector2f offset =
      steamrot::logic::positioning::camera::get_scene_view_offset(
          steamrot::SceneType::TEST, pool);
  REQUIRE(offset.x == 0.f);
  REQUIRE(offset.y == 0.f);
}

TEST_CASE(
    "positioning_camera::get_scene_view_offset: UNKNOWN returns zero offset",
    "[unit][positioning_camera]") {
  steamrot::EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 5);
  const sf::Vector2f offset =
      steamrot::logic::positioning::camera::get_scene_view_offset(
          steamrot::SceneType::UNKNOWN, pool);
  REQUIRE(offset.x == 0.f);
  REQUIRE(offset.y == 0.f);
}

TEST_CASE("positioning_camera::get_scene_view_offset: CRAFTING offsets by "
          "half toolbar width",
          "[unit][positioning_camera]") {
  // Toolbar width = 180 → offset = {-90, 0}
  auto pool = make_pool_with_left_toolbar(5, 0, 180.f);
  const sf::Vector2f offset =
      steamrot::logic::positioning::camera::get_scene_view_offset(
          steamrot::SceneType::CRAFTING, pool);
  REQUIRE(offset.x == -90.f);
  REQUIRE(offset.y == 0.f);
}

TEST_CASE("positioning_camera::get_scene_view_offset: CRAFTING with no "
          "toolbar returns zero offset",
          "[unit][positioning_camera]") {
  steamrot::EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 5);
  const sf::Vector2f offset =
      steamrot::logic::positioning::camera::get_scene_view_offset(
          steamrot::SceneType::CRAFTING, pool);
  REQUIRE(offset.x == 0.f);
  REQUIRE(offset.y == 0.f);
}

/////////////////////////////////////////////////
// get_world_view
/////////////////////////////////////////////////

TEST_CASE(
    "positioning_camera::get_world_view: default camera centres view on origin "
    "for non-CRAFTING scene",
    "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  sf::RenderTexture texture;
  steamrot::EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 5);

  auto resize_result = texture.resize({800u, 600u});
  if (!resize_result) {
    FAIL("Failed to resize RenderTexture");
  }

  const sf::View view = steamrot::logic::positioning::camera::get_world_view(
      camera_state, texture, steamrot::SceneType::TITLE, pool);

  REQUIRE(view.getCenter().x == 0.f);
  REQUIRE(view.getCenter().y == 0.f);
}

TEST_CASE(
    "positioning_camera::get_world_view: non-default position shifts view "
    "centre",
    "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_position = {100.f, 200.f};
  sf::RenderTexture texture;
  steamrot::EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 5);

  auto resize_result = texture.resize({800u, 600u});
  if (!resize_result) {
    FAIL("Failed to resize RenderTexture");
  }

  const sf::View view = steamrot::logic::positioning::camera::get_world_view(
      camera_state, texture, steamrot::SceneType::TITLE, pool);

  REQUIRE(view.getCenter().x == 100.f);
  REQUIRE(view.getCenter().y == 200.f);
}

TEST_CASE("positioning_camera::get_world_view: CRAFTING scene offsets view "
          "centre by half toolbar width",
          "[unit][positioning_camera]") {
  // Toolbar width = 180 → view centre at default pan = {-90, 0}.
  // Verifying: the usable-area screen centre {490, 300} maps back to world
  // {0, 0} through map_to_world_coords.
  steamrot::CameraState camera_state;
  sf::RenderTexture texture;
  auto pool = make_pool_with_left_toolbar(5, 0, 180.f);

  auto resize_result = texture.resize({800u, 600u});
  if (!resize_result) {
    FAIL("Failed to resize RenderTexture");
  }

  const sf::View view = steamrot::logic::positioning::camera::get_world_view(
      camera_state, texture, steamrot::SceneType::CRAFTING, pool);

  REQUIRE(view.getCenter().x == -90.f);
  REQUIRE(view.getCenter().y == 0.f);

  // The usable-area screen centre (toolbar_w + usable_w/2, h/2) = (490, 300)
  // must map to world {0, 0}.
  const sf::Vector2f world_origin =
      steamrot::logic::positioning::camera::map_to_world_coords(
          camera_state, {490, 300}, texture, steamrot::SceneType::CRAFTING,
          pool);
  REQUIRE(std::abs(world_origin.x) < 0.01f);
  REQUIRE(std::abs(world_origin.y) < 0.01f);
}

/////////////////////////////////////////////////
// map_to_world_coords
/////////////////////////////////////////////////

TEST_CASE("positioning_camera::map_to_world_coords: default camera maps screen "
          "centre to world origin",
          "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  sf::RenderTexture texture;
  steamrot::EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 5);
  constexpr unsigned int w = 800u;
  constexpr unsigned int h = 600u;

  auto resize_result = texture.resize({w, h});
  if (!resize_result) {
    FAIL("Failed to resize RenderTexture");
  }

  const sf::Vector2f world_pos =
      steamrot::logic::positioning::camera::map_to_world_coords(
          camera_state, {static_cast<int>(w / 2), static_cast<int>(h / 2)},
          texture, steamrot::SceneType::TITLE, pool);

  // Screen centre should map to world (0, 0) when camera is at origin
  REQUIRE(std::abs(world_pos.x) < 0.01f);
  REQUIRE(std::abs(world_pos.y) < 0.01f);
}
