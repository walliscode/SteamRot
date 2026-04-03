/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for GhostItemRenderLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GhostItemRenderLogic.h"
#include "GhostItemState.h"
#include "TestFixture.h"
#include <SFML/Graphics/Image.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GhostItemRenderLogic constructor does not throw",
          "[unit][GhostItemRenderLogic]") {
  steamrot::tests::TestFixture fixture;
  REQUIRE_NOTHROW(
      steamrot::logic::GhostItemRenderLogic(fixture.GetSceneContext()));
}

TEST_CASE("GhostItemRenderLogic::GetLogicType returns GhostItemRender",
          "[unit][GhostItemRenderLogic]") {
  steamrot::tests::TestFixture fixture;
  steamrot::logic::GhostItemRenderLogic logic(fixture.GetSceneContext());

  REQUIRE(logic.GetLogicType() == steamrot::LogicType::GhostItemRender);
}

TEST_CASE("GhostItemRenderLogic::ProcessLogic does nothing when ghost is "
          "inactive",
          "[unit][GhostItemRenderLogic]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &scene_context = fixture.GetSceneContext();
  scene_context.ghost_item_state.m_is_active = false;

  // Clear the texture to a known colour
  scene_context.scene_texture.clear(sf::Color::Black);
  scene_context.scene_texture.display();

  sf::Image before = scene_context.scene_texture.getTexture().copyToImage();

  steamrot::logic::GhostItemRenderLogic logic(scene_context);
  logic.RunLogic();
  scene_context.scene_texture.display();

  sf::Image after = scene_context.scene_texture.getTexture().copyToImage();

  // Textures should be identical — no pixels changed
  const auto size = before.getSize();
  bool any_change = false;
  for (unsigned int x = 0; x < size.x && !any_change; ++x) {
    for (unsigned int y = 0; y < size.y && !any_change; ++y) {
      if (before.getPixel({x, y}) != after.getPixel({x, y})) {
        any_change = true;
      }
    }
  }
  REQUIRE(!any_change);
}

TEST_CASE("GhostItemRenderLogic::ProcessLogic draws a ghost indicator when "
          "ghost is active",
          "[unit][GhostItemRenderLogic]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &scene_context = fixture.GetSceneContext();
  scene_context.ghost_item_state.m_is_active = true;
  scene_context.ghost_item_state.m_item_key = "fragment_x";
  // Place ghost in the centre of the texture
  scene_context.ghost_item_state.m_position = {400.f, 300.f};

  // Clear the texture to black
  scene_context.scene_texture.clear(sf::Color::Black);
  scene_context.scene_texture.display();

  steamrot::logic::GhostItemRenderLogic logic(scene_context);
  logic.RunLogic();
  scene_context.scene_texture.display();

  sf::Image after = scene_context.scene_texture.getTexture().copyToImage();

  // The ghost draws a 32x32 rectangle centred on (400, 300), so pixels in
  // the range [384, 416) x [284, 316) should be non-black.
  bool found_ghost_pixel = false;
  for (unsigned int x = 384; x < 416 && !found_ghost_pixel; ++x) {
    for (unsigned int y = 284; y < 316 && !found_ghost_pixel; ++y) {
      if (after.getPixel({x, y}) != sf::Color::Black) {
        found_ghost_pixel = true;
      }
    }
  }
  REQUIRE(found_ghost_pixel);
}
