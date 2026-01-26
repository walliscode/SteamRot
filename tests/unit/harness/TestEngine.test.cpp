/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"
#include "EventPacket.h"
#include "SceneData.h"
#include "TestData.h"
#include "containers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TestEngine initialises with a TestData object", "[TestEngine]") {
  steamrot::TestData test_data;
  steamrot::tests::TestEngine engine(test_data);
  SUCCEED("TestEngine initialised successfully");
}

TEST_CASE("TestEngine::StartUp assigns variables from TestData",
          "[TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 5;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto run_game_result = engine.RunGame();
  if (!run_game_result.has_value()) {
    FAIL("TestEngine::RunGame failed to start: " +
         run_game_result.error().message);
  }

  // Assert
  REQUIRE(engine.GetTargetTicks() == 5);
}

TEST_CASE("TestEngine::GetDataBank returns empty map initially",
          "[unit][TestEngine]") {
  steamrot::TestData test_data;
  steamrot::tests::TestEngine engine(test_data);

  const auto &data_bank = engine.GetDataBank();
  REQUIRE(data_bank.empty());
}

TEST_CASE("TestEngine::RunGame executes specified number of ticks",
          "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 4;
  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());
  // Assert
  REQUIRE(engine.GetCurrentTick() == 4);
}

TEST_CASE("TestEngine::RunGame captures snapshots in data bank",
          "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 3;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert
  const auto &data_bank = engine.GetDataBank();
  REQUIRE(data_bank.size() == 3);
  REQUIRE(data_bank.find(1) != data_bank.end());
  REQUIRE(data_bank.find(2) != data_bank.end());
  REQUIRE(data_bank.find(3) != data_bank.end());
  REQUIRE(data_bank.find(4) == data_bank.end());
}

TEST_CASE("TestEngine::StartUp loads EventBus from TestData",
          "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 1;

  // Create test events to load
  steamrot::EventPacket event1(steamrot::EventType::EventType_EVENT_QUIT_GAME,
                               steamrot::EventData{std::monostate{}}, 2);
  steamrot::EventPacket event2(
      steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
      steamrot::EventData{std::monostate{}}, 1);

  steamrot::EventBus test_event_bus{event1, event2};
  test_data.starting_engine_snapshot.global_event_bus = test_event_bus;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert
  const auto &engine_resources = engine.GetEngineResources();
  const auto &global_event_bus =
      engine_resources.event_handler.GetGlobalEventBus();

  // Events should be processed and in the global event bus
  REQUIRE(global_event_bus.size() == 2);
  REQUIRE(global_event_bus[0].event_type ==
          steamrot::EventType::EventType_EVENT_QUIT_GAME);
  REQUIRE(global_event_bus[1].event_type ==
          steamrot::EventType::EventType_EVENT_CHANGE_SCENE);
}

TEST_CASE("TestEngine::StartUp handles empty EventBus from TestData",
          "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 1;

  // Set empty event bus
  test_data.starting_engine_snapshot.global_event_bus = steamrot::EventBus{};

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert
  const auto &engine_resources = engine.GetEngineResources();
  const auto &global_event_bus =
      engine_resources.event_handler.GetGlobalEventBus();
  REQUIRE(global_event_bus.empty());
}

TEST_CASE("TestEngine::StartUp handles no EventBus in TestData",
          "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 1;

  // Don't set global_event_bus (leave as std::nullopt)
  REQUIRE(!test_data.starting_engine_snapshot.global_event_bus.has_value());

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert - should succeed without errors
  SUCCEED("TestEngine handled missing EventBus gracefully");
}

TEST_CASE("TestEngine::StartUp loads SceneCollection from TestData",
          "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 1;

  // Create a minimal SceneData for testing
  steamrot::SceneData scene_data;

  scene_data.scene_info.type = steamrot::SceneType::SceneType_TITLE;

  // Create an empty EntityMemoryPool for the scene
  auto entity_pool = std::make_shared<steamrot::EntityMemoryPool>();
  scene_data.entity_transport = entity_pool;

  steamrot::SceneCollectionData scene_collection{scene_data};
  test_data.starting_engine_snapshot.scene_collection_data = scene_collection;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert
  const auto &scene_manager = engine.GetSceneManager();
  const auto &scenes = scene_manager.GetScenes();

  // Should have loaded the scene from TestData
  REQUIRE(scenes.size() == 1);

  // Verify scene was configured correctly
  auto scene_it = scenes.begin();
  REQUIRE(scene_it->second != nullptr);
  // Note: We can't easily verify the scene name/type without additional getters
}

TEST_CASE("TestEngine::StartUp loads multiple scenes from TestData",
          "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 1;

  // Create multiple SceneData objects
  steamrot::SceneData scene_data1;
  scene_data1.scene_info.type = steamrot::SceneType::SceneType_TITLE;
  auto entity_pool1 = std::make_shared<steamrot::EntityMemoryPool>();
  scene_data1.entity_transport = entity_pool1;

  steamrot::SceneData scene_data2;
  scene_data2.scene_info.type = steamrot::SceneType::SceneType_CRAFTING;
  auto entity_pool2 = std::make_shared<steamrot::EntityMemoryPool>();
  scene_data2.entity_transport = entity_pool2;

  steamrot::SceneCollectionData scene_collection{scene_data1, scene_data2};
  test_data.starting_engine_snapshot.scene_collection_data = scene_collection;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert
  const auto &scene_manager = engine.GetSceneManager();
  const auto &scenes = scene_manager.GetScenes();

  // Should have loaded both scenes from TestData
  REQUIRE(scenes.size() == 2);
}

TEST_CASE("TestEngine::StartUp handles empty SceneCollection from TestData",
          "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 1;

  // Set empty scene collection
  test_data.starting_engine_snapshot.scene_collection_data =
      steamrot::SceneCollectionData{};

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert
  const auto &scene_manager = engine.GetSceneManager();
  const auto &scenes = scene_manager.GetScenes();

  // Should have no scenes loaded
  REQUIRE(scenes.empty());
}

TEST_CASE("TestEngine::StartUp configures all aspects from TestData",
          "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 5;

  // Configure EventBus
  steamrot::EventPacket event(steamrot::EventType::EventType_USER_INPUT,
                              steamrot::EventData{std::monostate{}}, 3);
  test_data.starting_engine_snapshot.global_event_bus =
      steamrot::EventBus{event};

  // Configure SceneCollection
  steamrot::SceneData scene_data;
  scene_data.scene_info.type = steamrot::SceneType::SceneType_TITLE;
  auto entity_pool = std::make_shared<steamrot::EntityMemoryPool>();
  scene_data.entity_transport = entity_pool;
  test_data.starting_engine_snapshot.scene_collection_data = {scene_data};

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert - verify all configuration aspects
  REQUIRE(engine.GetTargetTicks() == 5);

  const auto &engine_resources = engine.GetEngineResources();
  const auto &global_event_bus =
      engine_resources.event_handler.GetGlobalEventBus();
  REQUIRE(global_event_bus.size() == 1);

  const auto &scene_manager = engine.GetSceneManager();
  const auto &scenes = scene_manager.GetScenes();
  REQUIRE(scenes.size() == 1);
}
