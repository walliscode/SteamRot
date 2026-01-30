/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"
#include "EngineSnapshotEqualsMatcher.h"
#include "EventHandler.h"
#include "EventPacket.h"
#include "EventType.h"
#include "FlatbuffersEntityConfigurator.h"
#include "SceneData.h"
#include "TestData.h"
#include "harness_runner.h"
#include "uuid.h"
#include <catch2/catch_test_macros.hpp>
#include <utility>

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
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }
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
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }
  auto result = engine.RunGame();
  if (!result.has_value()) {
    FAIL("TestEngine::RunGame failed to start: " + result.error().message);
  }
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
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }
  auto result = engine.RunGame();
  if (!result.has_value()) {
    FAIL("TestEngine::RunGame failed to start: " + result.error().message);
  }

  // Assert
  const auto &data_bank = engine.GetDataBank();

  REQUIRE(data_bank.size() == 4);
  // zero position is the data before any ticks have run to check initial state
  REQUIRE(data_bank.find(0) != data_bank.end());
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
  steamrot::EventPacket event1(steamrot::EventType::QUIT_GAME,
                               steamrot::EventData{std::monostate{}}, 2);
  steamrot::EventPacket event2(steamrot::EventType::CHANGE_SCENE,
                               steamrot::EventData{std::monostate{}}, 1);

  steamrot::EventBus test_event_bus{event1, event2};
  test_data.starting_engine_snapshot.global_event_bus = test_event_bus;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.StartUp();
  REQUIRE(result.has_value());

  // Assert
  const auto &engine_resources = engine.GetEngineResources();
  const auto &global_event_bus =
      engine_resources.event_handler.GetGlobalEventBus();

  // Events should be processed and in the global event bus
  REQUIRE(global_event_bus.size() == 2);
  REQUIRE(global_event_bus[0].event_type == steamrot::EventType::QUIT_GAME);
  REQUIRE(global_event_bus[1].event_type == steamrot::EventType::CHANGE_SCENE);
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
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }
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
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }
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

  scene_data.scene_info.type = steamrot::SceneType::TITLE;
  scene_data.scene_info.id = uuids::uuid_system_generator{}();

  // set SceneResourcesConfig texture dimensions to non zero
  scene_data.scene_resources_config.texture_width = 512;
  scene_data.scene_resources_config.texture_height = 648;

  // Add SceneData to TestData
  test_data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_data));

  REQUIRE(test_data.starting_engine_snapshot.scene_collection_data.size() == 1);
  // set SceneResourcesConfig texture dimensions to non zero

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }

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
  scene_data1.scene_info.type = steamrot::SceneType::TITLE;
  scene_data1.scene_resources_config.texture_width = 256;
  scene_data1.scene_resources_config.texture_height = 256;

  steamrot::SceneData scene_data2;
  scene_data2.scene_info.type = steamrot::SceneType::CRAFTING;
  scene_data2.scene_resources_config.texture_width = 128;
  scene_data2.scene_resources_config.texture_height = 128;
  test_data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_data1));
  test_data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_data2));

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }
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
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }
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
  steamrot::EventPacket event(steamrot::EventType::USER_INPUT,
                              steamrot::EventData{std::monostate{}}, 3);
  test_data.starting_engine_snapshot.global_event_bus =
      steamrot::EventBus{event};

  // Configure SceneCollection
  steamrot::SceneData scene_data;
  scene_data.scene_info.type = steamrot::SceneType::TITLE;
  scene_data.scene_resources_config.texture_width = 300;
  scene_data.scene_resources_config.texture_height = 400;

  test_data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_data));

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }

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

TEST_CASE("TestEngine zero position snapshot matches minimal starting config",
          "[unit][TestEngine]") {
  // Arrange - Configure minimal starting state
  steamrot::TestData test_data;
  test_data.number_of_ticks = 2;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }

  // Assert - Zero position should match starting config
  const auto &data_bank = engine.GetDataBank();
  REQUIRE(data_bank.contains(0));

  REQUIRE_THAT(data_bank.at(0), steamrot::tests::EqualsEngineSnapshot(
                                    test_data.starting_engine_snapshot));
}

TEST_CASE(
    "TestEngine zero position snapshot matches starting config with EventBus",
    "[unit][TestEngine]") {
  // Arrange - Configure starting snapshot with EventBus
  steamrot::TestData test_data;
  test_data.number_of_ticks = 2;

  // Configure specific starting event bus
  steamrot::EventPacket event1(steamrot::EventType::USER_INPUT,
                               steamrot::EventData{std::monostate{}}, 1);
  steamrot::EventPacket event2(steamrot::EventType::QUIT_GAME,
                               steamrot::EventData{std::monostate{}}, 2);
  test_data.starting_engine_snapshot.global_event_bus =
      steamrot::EventBus{event1, event2};

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }

  // Assert - Zero position should match starting config
  const auto &data_bank = engine.GetDataBank();
  REQUIRE(data_bank.contains(0));

  REQUIRE_THAT(data_bank.at(0), steamrot::tests::EqualsEngineSnapshot(
                                    test_data.starting_engine_snapshot));
}

TEST_CASE("TestEngine zero position snapshot matches starting config with "
          "SceneCollection",
          "[unit][TestEngine]") {
  // Arrange - Configure starting snapshot with SceneCollection
  steamrot::TestData test_data;
  test_data.number_of_ticks = 2;
  steamrot::EventHandler mock_event_handler; // for EMP conversion

  // Configure specific starting scene
  steamrot::SceneData scene_data;
  scene_data.scene_info.type = steamrot::SceneType::TITLE;
  scene_data.scene_info.id = uuids::uuid_system_generator{}();
  scene_data.scene_resources_config.texture_width = 512;
  scene_data.scene_resources_config.texture_height = 648;
  scene_data.entity_configurator =
      std::make_unique<steamrot::FlatbuffersEntityConfigurator>(
          mock_event_handler);
  test_data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_data));

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }

  // Convert all scene entity data before comparison
  auto convert_result = steamrot::tests::ConvertAllSceneEntityData(
      test_data.starting_engine_snapshot);
  if (!convert_result.has_value()) {
    FAIL("ConvertAllSceneEntityData failed: " + convert_result.error().message);
  }

  // Assert - Zero position should match starting config
  const auto &data_bank = engine.GetDataBank();
  REQUIRE(data_bank.contains(0));

  REQUIRE_THAT(data_bank.at(0), steamrot::tests::EqualsEngineSnapshot(
                                    test_data.starting_engine_snapshot));
}

TEST_CASE("TestEngine zero position snapshot matches complete starting config "
          "with events and scenes",
          "[unit][TestEngine]") {
  // Arrange - Configure complete starting state
  steamrot::TestData test_data;
  test_data.number_of_ticks = 3;

  // Configure EventBus
  steamrot::EventPacket event(steamrot::EventType::CHANGE_SCENE,
                              steamrot::EventData{std::monostate{}}, 1);
  test_data.starting_engine_snapshot.global_event_bus =
      steamrot::EventBus{event};

  // Configure SceneCollection
  steamrot::SceneData scene_data;
  scene_data.scene_info.type = steamrot::SceneType::CRAFTING;
  scene_data.scene_info.id = uuids::uuid_system_generator{}();
  scene_data.scene_resources_config.texture_width = 300;
  scene_data.scene_resources_config.texture_height = 400;

  test_data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_data));

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }

  // Assert - Zero position should match starting config
  const auto &data_bank = engine.GetDataBank();
  REQUIRE(data_bank.contains(0));

  REQUIRE_THAT(data_bank.at(0), steamrot::tests::EqualsEngineSnapshot(
                                    test_data.starting_engine_snapshot));
}

TEST_CASE("TestEngine zero position snapshot matches starting config with "
          "multiple scenes",
          "[unit][TestEngine]") {
  // Arrange - Configure starting snapshot with multiple scenes
  steamrot::TestData test_data;
  test_data.number_of_ticks = 2;

  // Configure first scene
  steamrot::SceneData scene_data1;
  scene_data1.scene_info.type = steamrot::SceneType::TITLE;
  scene_data1.scene_info.id = uuids::uuid_system_generator{}();
  scene_data1.scene_resources_config.texture_width = 256;
  scene_data1.scene_resources_config.texture_height = 256;

  // Configure second scene
  steamrot::SceneData scene_data2;
  scene_data2.scene_info.type = steamrot::SceneType::CRAFTING;
  scene_data2.scene_info.id = uuids::uuid_system_generator{}();
  scene_data2.scene_resources_config.texture_width = 128;
  scene_data2.scene_resources_config.texture_height = 128;

  test_data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_data1));
  test_data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_data2));

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }

  // Assert - Zero position should match starting config
  const auto &data_bank = engine.GetDataBank();
  REQUIRE(data_bank.contains(0));

  REQUIRE_THAT(data_bank.at(0), steamrot::tests::EqualsEngineSnapshot(
                                    test_data.starting_engine_snapshot));
}

TEST_CASE("TestEngine zero position snapshot matches empty EventBus config",
          "[unit][TestEngine]") {
  // Arrange - Configure with empty EventBus
  steamrot::TestData test_data;
  test_data.number_of_ticks = 1;

  // Set empty event bus explicitly
  test_data.starting_engine_snapshot.global_event_bus = steamrot::EventBus{};

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto startup_result = engine.StartUp();
  if (!startup_result.has_value()) {
    FAIL("TestEngine::StartUp failed: " + startup_result.error().message);
  }

  // Assert - Zero position should match starting config with empty bus
  const auto &data_bank = engine.GetDataBank();
  REQUIRE(data_bank.contains(0));

  REQUIRE_THAT(data_bank.at(0), steamrot::tests::EqualsEngineSnapshot(
                                    test_data.starting_engine_snapshot));
}
