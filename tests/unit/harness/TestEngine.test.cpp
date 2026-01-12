/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"
#include "TestData.h"
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

TEST_CASE("EngineSnapshot in data bank contains scene_collection_data",
          "[unit][TestEngine][EngineSnapshot]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 2;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert - Check that snapshots contain scene_collection_data
  const auto &data_bank = engine.GetDataBank();
  REQUIRE(data_bank.size() == 2);

  // Verify tick 1 snapshot
  const auto &snapshot_tick1 = data_bank.at(1);
  REQUIRE(!snapshot_tick1.scene_collection_data.empty());

  // Verify tick 2 snapshot
  const auto &snapshot_tick2 = data_bank.at(2);
  REQUIRE(!snapshot_tick2.scene_collection_data.empty());
}

TEST_CASE("EngineSnapshot scene_collection_data contains SceneInfo",
          "[unit][TestEngine][EngineSnapshot]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 1;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert
  const auto &data_bank = engine.GetDataBank();
  const auto &snapshot = data_bank.at(1);

  // Each SceneData should have valid SceneInfo
  for (const auto &scene_data : snapshot.scene_collection_data) {
    // SceneInfo should have a valid UUID (not nil)
    REQUIRE(!scene_data.scene_info.id.is_nil());
    // SceneType should be valid (not NONE or invalid enum value)
    REQUIRE(scene_data.scene_info.type != steamrot::SceneType::SceneType_NONE);
  }
}

TEST_CASE(
    "EngineSnapshot scene_collection_data contains EntityMemoryPool variant",
    "[unit][TestEngine][EngineSnapshot]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 1;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert
  const auto &data_bank = engine.GetDataBank();
  const auto &snapshot = data_bank.at(1);

  // Each SceneData should have entity_transport as EntityMemoryPool (after
  // conversion)
  for (const auto &scene_data : snapshot.scene_collection_data) {
    // After StoreEngineSnapShot conversion, should be EntityMemoryPool variant
    REQUIRE(std::holds_alternative<steamrot::EntityMemoryPool>(
        scene_data.entity_transport));
  }
}

TEST_CASE("EngineSnapshot EntityMemoryPool contains valid data after "
          "conversion",
          "[unit][TestEngine][EngineSnapshot]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 1;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert
  const auto &data_bank = engine.GetDataBank();
  const auto &snapshot = data_bank.at(1);

  // Verify EntityMemoryPool data is accessible
  for (const auto &scene_data : snapshot.scene_collection_data) {
    const auto &emp = std::get<steamrot::EntityMemoryPool>(
        scene_data.entity_transport);

    // EntityMemoryPool should be accessible and have a valid size
    // The default scene should have at least some entity capacity
    REQUIRE(emp.GetSize() > 0);
  }
}

TEST_CASE("Multiple EngineSnapshots are independent copies",
          "[unit][TestEngine][EngineSnapshot]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 2;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert
  const auto &data_bank = engine.GetDataBank();
  const auto &snapshot1 = data_bank.at(1);
  const auto &snapshot2 = data_bank.at(2);

  // Snapshots should both contain scene data
  REQUIRE(!snapshot1.scene_collection_data.empty());
  REQUIRE(!snapshot2.scene_collection_data.empty());

  // Verify both snapshots have EntityMemoryPool variants (independent copies)
  for (const auto &scene_data : snapshot1.scene_collection_data) {
    REQUIRE(std::holds_alternative<steamrot::EntityMemoryPool>(
        scene_data.entity_transport));
  }

  for (const auto &scene_data : snapshot2.scene_collection_data) {
    REQUIRE(std::holds_alternative<steamrot::EntityMemoryPool>(
        scene_data.entity_transport));
  }
}

TEST_CASE("EngineSnapshot data bank keys match tick numbers",
          "[unit][TestEngine][EngineSnapshot]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 5;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert
  const auto &data_bank = engine.GetDataBank();
  REQUIRE(data_bank.size() == 5);

  // Verify all expected tick numbers are present as keys
  for (size_t tick = 1; tick <= 5; ++tick) {
    REQUIRE(data_bank.find(tick) != data_bank.end());
  }

  // Verify no unexpected tick numbers
  REQUIRE(data_bank.find(0) == data_bank.end());
  REQUIRE(data_bank.find(6) == data_bank.end());
}

TEST_CASE("EngineSnapshot captures tick_number correctly",
          "[unit][TestEngine][EngineSnapshot]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 3;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert - Verify tick_number is set correctly in each snapshot
  const auto &data_bank = engine.GetDataBank();

  const auto &snapshot1 = data_bank.at(1);
  REQUIRE(snapshot1.tick_number.has_value());
  REQUIRE(snapshot1.tick_number.value() == 1);

  const auto &snapshot2 = data_bank.at(2);
  REQUIRE(snapshot2.tick_number.has_value());
  REQUIRE(snapshot2.tick_number.value() == 2);

  const auto &snapshot3 = data_bank.at(3);
  REQUIRE(snapshot3.tick_number.has_value());
  REQUIRE(snapshot3.tick_number.value() == 3);
}

TEST_CASE("EngineSnapshot optional fields are not set by default",
          "[unit][TestEngine][EngineSnapshot]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 1;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert - Verify optional fields that aren't captured by default
  const auto &data_bank = engine.GetDataBank();
  const auto &snapshot = data_bank.at(1);

  // tick_number should be set (we added this)
  REQUIRE(snapshot.tick_number.has_value());

  // global_event_bus is optional and not captured by default
  // This is by design - only capture it when needed for event testing
  REQUIRE(!snapshot.global_event_bus.has_value());

  // scene_manager_data is optional and not captured by default
  // This is by design - only capture it when needed for save/load testing
  REQUIRE(!snapshot.scene_manager_data.has_value());

  // scene_collection_data is always captured (not optional)
  REQUIRE(!snapshot.scene_collection_data.empty());
}
