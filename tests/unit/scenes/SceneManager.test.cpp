/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SceneManager
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneManager.h"
#include "Subscriber.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SceneManager::SceneManager initializes correctly",
          "[SceneManager]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  // Act
  steamrot::SceneManager scene_manager(fixture.GetGameContext());
  // Assert
  SUCCEED("SceneManager initialized without errors");
}

TEST_CASE("SceneManager::StartUp loads configuration successfully",
          "[SceneManager]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneManager scene_manager(fixture.GetGameContext());
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().engine_resources.event_handler;

  // initial check - no subscribers
  REQUIRE(scene_manager.GetSubscriptions().empty());
  REQUIRE(event_handler.GetSubcriberRegister().empty());
  // Act
  auto result = scene_manager.StartUp();
  // Assert
  if (!result.has_value())
    FAIL(result.error().message);

  // check subscribers
  const std::vector<std::shared_ptr<steamrot::Subscriber>> &subscriptions =
      scene_manager.GetSubscriptions();
  REQUIRE(subscriptions.size() == 1);
  REQUIRE(subscriptions[0]->m_trigger_event_type ==
          steamrot::EventType::EventType_EVENT_CHANGE_SCENE);

  // check subscribers have been registered with the EventHandler
  const auto &registered_subscribers = event_handler.GetSubcriberRegister();
  REQUIRE(registered_subscribers.size() == 1);
}
