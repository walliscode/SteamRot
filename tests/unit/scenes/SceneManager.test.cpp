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
}
