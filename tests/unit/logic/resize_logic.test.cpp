/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for resize_logic free functions and ResizeLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPayload.h"
#include "EventType.h"
#include "ResizeLogic.h"
#include "TestFixture.h"
#include "resize_logic.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <catch2/catch_test_macros.hpp>

// ---------------------------------------------------------------------------
// resize_logic free functions
// ---------------------------------------------------------------------------

TEST_CASE("resize::ResizeSceneTexture returns success for valid dimensions",
          "[unit][resize_logic]") {
  sf::RenderTexture texture;
  // create a render texture with initial size
  REQUIRE(texture.resize({100u, 100u}));

  auto result =
      steamrot::logic::resize::ResizeSceneTexture(texture, {200u, 150u});

  REQUIRE(result.has_value());
  REQUIRE(texture.getSize().x == 200u);
  REQUIRE(texture.getSize().y == 150u);
}

TEST_CASE("resize::ResizeSceneTexture updates texture to the given size",
          "[unit][resize_logic]") {
  sf::RenderTexture texture;
  REQUIRE(texture.resize({50u, 50u}));

  steamrot::logic::resize::ResizeSceneTexture(texture, {1920u, 1080u});

  REQUIRE(texture.getSize().x == 1920u);
  REQUIRE(texture.getSize().y == 1080u);
}

// ---------------------------------------------------------------------------
// ResizeLogic
// ---------------------------------------------------------------------------

TEST_CASE("ResizeLogic constructs without errors", "[unit][ResizeLogic]") {
  steamrot::tests::TestFixture fixture;
  steamrot::logic::ResizeLogic resize_logic(fixture.GetSceneContext());
  SUCCEED("ResizeLogic constructed successfully");
}

TEST_CASE("ResizeLogic::GetLogicType returns Resize", "[unit][ResizeLogic]") {
  steamrot::tests::TestFixture fixture;
  steamrot::logic::ResizeLogic resize_logic(fixture.GetSceneContext());
  REQUIRE(resize_logic.GetLogicType() == steamrot::LogicType::Resize);
}

TEST_CASE("ResizeLogic registers one SYSTEM RESIZE subscriber on construction",
          "[unit][ResizeLogic]") {
  steamrot::tests::TestFixture fixture;
  steamrot::logic::ResizeLogic resize_logic(fixture.GetSceneContext());

  const auto &subscribers = resize_logic.GetSubscribers();
  REQUIRE(subscribers.size() == 1);

  const auto &subscriber = subscribers[0];
  REQUIRE(subscriber->event_type == steamrot::EventType::SYSTEM);

  auto *payload =
      std::get_if<steamrot::SystemPayload>(&subscriber->filter_payload);
  REQUIRE(payload != nullptr);
  REQUIRE(payload->action == steamrot::SystemPayload::SystemAction::RESIZE);
}

TEST_CASE(
    "ResizeLogic::RunLogic resizes scene texture when RESIZE event is active",
    "[unit][ResizeLogic]") {
  steamrot::tests::TestFixture fixture;

  // Initialise the fixture's scene texture to a known size
  sf::RenderTexture &scene_texture =
      fixture.GetSceneContext().scene_texture;
  REQUIRE(scene_texture.resize({100u, 100u}));

  steamrot::logic::ResizeLogic resize_logic(fixture.GetSceneContext());

  // Manually activate the subscriber with a RESIZE payload carrying new size
  auto &subscribers = resize_logic.GetSubscribers();
  REQUIRE(subscribers.size() == 1);

  subscribers[0]->m_active = true;
  subscribers[0]->captured_payload =
      steamrot::SystemPayload{steamrot::SystemPayload::SystemAction::RESIZE,
                              sf::Vector2u{640u, 480u}};

  resize_logic.RunLogic();

  REQUIRE(scene_texture.getSize().x == 640u);
  REQUIRE(scene_texture.getSize().y == 480u);
}

TEST_CASE("ResizeLogic::RunLogic does nothing when subscriber is inactive",
          "[unit][ResizeLogic]") {
  steamrot::tests::TestFixture fixture;

  sf::RenderTexture &scene_texture =
      fixture.GetSceneContext().scene_texture;
  REQUIRE(scene_texture.resize({100u, 100u}));

  steamrot::logic::ResizeLogic resize_logic(fixture.GetSceneContext());

  // Subscriber is inactive (default)
  resize_logic.RunLogic();

  REQUIRE(scene_texture.getSize().x == 100u);
  REQUIRE(scene_texture.getSize().y == 100u);
}
