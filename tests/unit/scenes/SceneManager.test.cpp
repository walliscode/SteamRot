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
#include "scene_types_generated.h"
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

TEST_CASE("SceneManager::AddScenesFromSceneCollectionData clears exisiting "
          "scenes and adds new ones",
          "[SceneManager]") {

  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneManager scene_manager(fixture.GetGameContext());
  const auto &scene_map = scene_manager.GetScenes();

  auto scene_add_result =
      scene_manager.AddSceneFromDefault(steamrot::SceneType_TEST);
  if (!scene_add_result.has_value()) {
    FAIL(scene_add_result.error().message);
  }
  // check that there is one scene in the map
  REQUIRE(scene_map.size() == 1);

  // create empty SceneCollectionData
  steamrot::SceneCollectionData scene_collection_data;

  // Act
  auto scene_collection_add_result =
      scene_manager.AddScenesFromSceneCollectionData(scene_collection_data);
  if (!scene_collection_add_result.has_value()) {
    FAIL(scene_collection_add_result.error().message);
  }
  // Assert
  // check that the scene map is now empty
  REQUIRE(scene_map.empty());
}
