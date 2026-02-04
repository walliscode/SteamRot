/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the harness runner functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "harness_runner.h"
#include "EngineSnapshotEqualsMatcher.h"
#include "EventHandler.h"
#include "FlatbuffersEntityConfigurator.h"
#include "TestEngine.h"
#include "containers.h"
#include "entities_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <variant>

TEST_CASE("RunHarnessTests returns unexpected when no adjacent data directory "
          "is found",
          "[harness_runner]") {
  // Arrange
  std::filesystem::path temp_dir = std::filesystem::temp_directory_path();

  // Act
  auto result = steamrot::tests::RunHarnessTests(temp_dir);

  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().message ==
          std::format("No adjacent 'data' directory found at {}",
                      (temp_dir / "data").string()));
}

TEST_CASE(
    "RunHarnessTests returns unexpected when no test data is found in data "
    "directory",
    "[harness_runner]") {
  // Arrange
  std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
  std::filesystem::path data_dir = temp_dir / "data";
  // Create the data directory
  std::filesystem::create_directory(data_dir);
  // Act
  auto result = steamrot::tests::RunHarnessTests(temp_dir);
  // Clean up the data directory
  std::filesystem::remove(data_dir);
  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().message ==
          "No test data found in the 'data' directory.");
}

TEST_CASE(
    "ConvertEMPData returns unexpected when SceneData variant type is invalid",
    "[harness_runner]") {
  // Arrange
  steamrot::SceneData scene_data;
  steamrot::EntityMemoryPool emp;
  scene_data.entity_transport = emp;

  // Act
  auto result = steamrot::tests::ConvertEntityTransportVariant(scene_data);
  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "Invalid variant type in SceneData, "
                                    "EntityCollectionFbs pointer is required");
}

TEST_CASE(
    ""
    "ConvertEMPData returns unexpected when SceneData entity_configurator "
    "is null",
    "[harness_runner]") {
  // Arrange
  steamrot::SceneData scene_data;
  const steamrot::EntityCollectionFbs *ec_fbs = nullptr;
  scene_data.entity_transport = ec_fbs;
  scene_data.entity_configurator = nullptr;

  // Act
  auto result = steamrot::tests::ConvertEntityTransportVariant(scene_data);
  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "SceneData entity_configurator is null.");
}

TEST_CASE("ConvertEMPData converts EntityCollectionFbs to EntityMemoryPool "
          "successfully",
          "[harness_runner]") {
  // Arrange
  steamrot::EventHandler event_handler;
  steamrot::SceneData scene_data;
  // set up EntityCollectionFbs pointer using flatbuffers builder
  flatbuffers::FlatBufferBuilder builder;
  // create empty vector of EntityDataFbs offsets

  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  auto entities_vector = builder.CreateVector(entities);
  auto ec_fbs_offset =
      steamrot::CreateEntityCollectionFbs(builder, entities_vector, 10);
  builder.Finish(ec_fbs_offset);
  auto entity_collection_fbs =
      steamrot::GetEntityCollectionFbs(builder.GetBufferPointer());

  scene_data.entity_transport = entity_collection_fbs;
  scene_data.entity_configurator =
      std::make_unique<steamrot::FlatbuffersEntityConfigurator>(event_handler);

  REQUIRE(std::holds_alternative<const steamrot::EntityCollectionFbs *>(
      scene_data.entity_transport));

  // Act
  auto result = steamrot::tests::ConvertEntityTransportVariant(scene_data);
  // Assert
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  REQUIRE(std::holds_alternative<steamrot::EntityMemoryPool>(
      scene_data.entity_transport));
}

TEST_CASE(
    "ConvertAllEMPData returns unexpected when any SceneData variant type is "
    "invalid",
    "[harness_runner]") {
  // Arrange
  steamrot::EngineSnapshot engine_snapshot;
  steamrot::EventHandler event_handler;
  steamrot::SceneData scene_data_valid;
  steamrot::SceneData scene_data_invalid;

  // set up valid SceneData
  flatbuffers::FlatBufferBuilder builder;
  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  auto entities_vector = builder.CreateVector(entities);
  auto ec_fbs_offset =
      steamrot::CreateEntityCollectionFbs(builder, entities_vector, 10);
  builder.Finish(ec_fbs_offset);
  auto entity_collection_fbs =
      steamrot::GetEntityCollectionFbs(builder.GetBufferPointer());

  scene_data_valid.entity_transport = entity_collection_fbs;

  scene_data_valid.entity_configurator =
      std::make_unique<steamrot::FlatbuffersEntityConfigurator>(event_handler);

  // set up invalid SceneData
  steamrot::EntityMemoryPool emp;
  scene_data_invalid.entity_transport = emp;

  engine_snapshot.scene_collection_data.push_back(std::move(scene_data_valid));
  engine_snapshot.scene_collection_data.push_back(
      std::move(scene_data_invalid));

  // Act
  auto result =
      steamrot::tests::ConvertAllEntityTransportVariants(engine_snapshot);
  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "Invalid variant type in SceneData, "
                                    "EntityCollectionFbs pointer is required");
}

TEST_CASE("ConvertAllEMPData converts all SceneData EntityCollectionFbs to "
          "EntityMemoryPool successfully",
          "[harness_runner]") {
  // Arrange
  steamrot::EngineSnapshot engine_snapshot;
  steamrot::EventHandler event_handler;
  for (int i = 0; i < 3; ++i) {
    steamrot::SceneData scene_data;
    // set up EntityCollectionFbs pointer using flatbuffers builder
    flatbuffers::FlatBufferBuilder builder;
    // create empty vector of EntityDataFbs offsets
    std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
    auto entities_vector = builder.CreateVector(entities);
    auto ec_fbs_offset =
        steamrot::CreateEntityCollectionFbs(builder, entities_vector, 10);
    builder.Finish(ec_fbs_offset);
    auto entity_collection_fbs =
        steamrot::GetEntityCollectionFbs(builder.GetBufferPointer());
    scene_data.entity_transport = entity_collection_fbs;
    scene_data.entity_configurator =
        std::make_unique<steamrot::FlatbuffersEntityConfigurator>(
            event_handler);
    engine_snapshot.scene_collection_data.push_back(std::move(scene_data));
  }
  // Act
  auto result =
      steamrot::tests::ConvertAllEntityTransportVariants(engine_snapshot);
  // Assert
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  for (const auto &scene_data : engine_snapshot.scene_collection_data) {
    REQUIRE(std::holds_alternative<steamrot::EntityMemoryPool>(
        scene_data.entity_transport));
  }
}

TEST_CASE("ConvertAllEntityTransportVariantsInTestData converts the starting "
          "and expected engine_snapshots") {
  // Arrange
  steamrot::TestData test_data;
  steamrot::EventHandler event_handler;
  // set up starting EngineSnapshot
  {
    steamrot::EngineSnapshot starting_snapshot;
    for (int i = 0; i < 2; ++i) {
      steamrot::SceneData scene_data;
      // set up EntityCollectionFbs pointer using flatbuffers builder
      flatbuffers::FlatBufferBuilder builder;
      // create empty vector of EntityDataFbs offsets
      std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
      auto entities_vector = builder.CreateVector(entities);
      auto ec_fbs_offset =
          steamrot::CreateEntityCollectionFbs(builder, entities_vector, 10);
      builder.Finish(ec_fbs_offset);
      auto entity_collection_fbs =
          steamrot::GetEntityCollectionFbs(builder.GetBufferPointer());
      scene_data.entity_transport = entity_collection_fbs;
      scene_data.entity_configurator =
          std::make_unique<steamrot::FlatbuffersEntityConfigurator>(
              event_handler);
      starting_snapshot.scene_collection_data.push_back(std::move(scene_data));
    }
    test_data.starting_engine_snapshot = std::move(starting_snapshot);
  }
  // set up expected EngineSnapshots
  for (int j = 0; j < 2; ++j) {
    steamrot::EngineSnapshot expected_snapshot;
    for (int i = 0; i < 2; ++i) {
      steamrot::SceneData scene_data;
      // set up EntityCollectionFbs pointer using flatbuffers builder
      flatbuffers::FlatBufferBuilder builder;
      // create empty vector of EntityDataFbs offsets
      std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
      auto entities_vector = builder.CreateVector(entities);
      auto ec_fbs_offset =
          steamrot::CreateEntityCollectionFbs(builder, entities_vector, 10);
      builder.Finish(ec_fbs_offset);
      auto entity_collection_fbs =
          steamrot::GetEntityCollectionFbs(builder.GetBufferPointer());
      scene_data.entity_transport = entity_collection_fbs;
      scene_data.entity_configurator =
          std::make_unique<steamrot::FlatbuffersEntityConfigurator>(
              event_handler);
      expected_snapshot.scene_collection_data.push_back(std::move(scene_data));
    }
    test_data.expected_engine_snapshots[j] = std::move(expected_snapshot);
  }

  // Act
  auto result =
      steamrot::tests::ConvertAllEntityTransportVariantsInTestData(test_data);
  // Assert
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  // check starting snapshot
  for (const auto &scene_data :
       test_data.starting_engine_snapshot.scene_collection_data) {
    REQUIRE(std::holds_alternative<steamrot::EntityMemoryPool>(
        scene_data.entity_transport));
  }

  // check expected snapshots
  for (const auto &[tick, engine_snapshot] :
       test_data.expected_engine_snapshots) {
    for (const auto &scene_data : engine_snapshot.scene_collection_data) {
      REQUIRE(std::holds_alternative<steamrot::EntityMemoryPool>(
          scene_data.entity_transport));
    }
  }
}

TEST_CASE("CompareEngineSnapshots succeeds when snapshots match",
          "[unit][harness_runner]") {
  // Arrange
  steamrot::EngineSnapshot actual;
  steamrot::EngineSnapshot expected;

  actual.tick_number = 1;
  expected.tick_number = 1;

  // Act
  auto result =
      steamrot::tests::CompareEngineSnapshots(actual, expected, "test_name", 1);

  // Assert
  REQUIRE(result.has_value());
}

TEST_CASE("CompareEngineSnapshots fails when tick numbers mismatch",
          "[unit][harness_runner]") {
  // Arrange
  steamrot::EngineSnapshot actual;
  steamrot::EngineSnapshot expected;

  actual.tick_number = 2;
  expected.tick_number = 1;

  // Act
  auto result =
      steamrot::tests::CompareEngineSnapshots(actual, expected, "test_name", 1);

  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().message.find("Tick number mismatch") !=
          std::string::npos);
}

TEST_CASE("EngineSnapshotEqualsMatcher directly detects tick number mismatch",
          "[unit][harness_runner]") {
  // Arrange
  steamrot::EngineSnapshot actual;
  steamrot::EngineSnapshot expected;

  actual.tick_number = 2;
  expected.tick_number = 1;

  // Act & Assert
  // Using the matcher directly to verify mismatch detection
  REQUIRE_THAT(actual, !steamrot::tests::EqualsEngineSnapshot(expected));
}

TEST_CASE("EngineSnapshotEqualsMatcher includes context in error message",
          "[unit][harness_runner]") {
  // Arrange
  steamrot::EngineSnapshot actual;
  steamrot::EngineSnapshot expected;

  actual.tick_number = 2;
  expected.tick_number = 1;

  steamrot::tests::TestContext context{"test_name", "test description", 1, 10};

  // Act
  auto matcher =
      steamrot::tests::EngineSnapshotEqualsMatcher(expected, context);
  matcher.match(actual);

  // Assert
  std::string description = matcher.describe();
  REQUIRE(description.find("test_name") != std::string::npos);
  REQUIRE(description.find("Tick number mismatch") != std::string::npos);
  REQUIRE(description.find("expected 1") != std::string::npos);
  REQUIRE(description.find("got 2") != std::string::npos);
}

TEST_CASE("RunSnapshotComparisons succeeds when all snapshots match",
          "[unit][harness_runner]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.meta_data.test_name = "test_run";
  test_data.number_of_ticks = 2;

  // Set up starting snapshot (tick 0)
  steamrot::EngineSnapshot starting_snapshot;
  starting_snapshot.tick_number = 0;
  test_data.starting_engine_snapshot = std::move(starting_snapshot);

  // Set up expected snapshots
  for (size_t i = 1; i <= 2; ++i) {
    steamrot::EngineSnapshot expected_snapshot;
    expected_snapshot.tick_number = i;
    test_data.expected_engine_snapshots[i] = std::move(expected_snapshot);
  }

  // Create TestEngine with test data
  steamrot::tests::TestEngine test_engine{test_data};

  // Start up the engine to populate the data bank properly
  auto &mutable_engine = const_cast<steamrot::tests::TestEngine &>(test_engine);

  auto startup_result = mutable_engine.StartUp();
  REQUIRE(startup_result.has_value());

  auto run_result = mutable_engine.RunGame();
  REQUIRE(run_result.has_value());

  // Act
  auto result = steamrot::tests::RunSnapshotComparisons(test_engine, test_data);

  // Assert
  REQUIRE(result.has_value());
}

TEST_CASE("RunSnapshotComparisons returns unexpected when snapshot is missing",
          "[unit][harness_runner]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.meta_data.test_name = "test_missing";
  test_data.number_of_ticks = 1;

  // Set up expected snapshot for tick 5 (which won't exist in data bank)
  steamrot::EngineSnapshot expected_snapshot;
  expected_snapshot.tick_number = 5;
  test_data.expected_engine_snapshots[5] = std::move(expected_snapshot);

  // Create TestEngine
  steamrot::tests::TestEngine test_engine{test_data};

  // Start up the engine (this will create tick 0 snapshot)
  auto startup_result =
      const_cast<steamrot::tests::TestEngine &>(test_engine).StartUp();
  REQUIRE(startup_result.has_value());

  // Act
  auto result = steamrot::tests::RunSnapshotComparisons(test_engine, test_data);

  // Assert
  // This should return an error because tick 5 doesn't exist in data bank
  REQUIRE(!result.has_value());
  REQUIRE(result.error().message.find("test_missing") != std::string::npos);
  REQUIRE(result.error().message.find("tick 5") != std::string::npos);
  REQUIRE(result.error().message.find("not found") != std::string::npos);
}
