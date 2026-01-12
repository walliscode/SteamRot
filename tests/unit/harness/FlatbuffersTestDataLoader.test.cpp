/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the FlatbuffersTestDataLoader class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersTestDataLoader.h"
#include "engine_snapshot_generated.h"
#include "engine_state_generated.h"
#include "simulation_data_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersTestDataLoader constructor sets object directory path "
          "correctly",
          "[FlatbuffersTestDataLoader]") {
  // Arrange
  std::filesystem::path obj_dir_path(__FILE__);

  // Act
  FlatbuffersTestDataLoader loader(obj_dir_path.parent_path());

  // Assert
  REQUIRE(loader.GetObjectDirectoryPath() == obj_dir_path.parent_path());
}

TEST_CASE("FlatbuffersTestDataLoader::GetAdjacentDataDirectoryPath returns "
          "the correct path when data directory exists",
          "[FlatbuffersTestDataLoader]") {
  // Arrange
  std::filesystem::path obj_dir_path(__FILE__);
  FlatbuffersTestDataLoader loader(obj_dir_path.parent_path());
  // Act
  auto result = loader.GetAdjacentDataDirectoryPath();
  // Assert
  REQUIRE(result.has_value());
  REQUIRE(result.value() == obj_dir_path.parent_path() / "data");
}

TEST_CASE(
    "FlatbuffersTestDataLoader::GetAdjacentDataDirectoryPath returns error "
    "when data directory does not exist",
    "[FlatbuffersTestDataLoader]") {
  // Arrange
  std::filesystem::path obj_dir_path =
      std::filesystem::temp_directory_path() / "non_existent_directory";
  FlatbuffersTestDataLoader loader(obj_dir_path);
  // Act
  auto result = loader.GetAdjacentDataDirectoryPath();
  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::DirectoryNotFound);
}

TEST_CASE("FlatbuffersTestDataLoader::LoadTestDataFbs returns error when data "
          "directory does not exist",
          "[FlatbuffersTestDataLoader]") {
  // Arrange
  std::filesystem::path obj_dir_path =
      std::filesystem::temp_directory_path() / "non_existent_directory";
  FlatbuffersTestDataLoader loader(obj_dir_path);
  // Act
  auto result = loader.LoadTestDataFbs();
  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::DirectoryNotFound);
}

TEST_CASE(
    "FlatbuffersTestDataLoader::LoadTestDataFbs returns empty vector when "
    "data directory is empty",
    "[FlatbuffersTestDataLoader]") {
  // Arrange
  std::filesystem::path temp_dir =
      std::filesystem::temp_directory_path() / "empty_data_directory";
  std::filesystem::create_directories(temp_dir / "data");
  FlatbuffersTestDataLoader loader(temp_dir);
  // Act
  auto result = loader.LoadTestDataFbs();
  // Assert
  REQUIRE(result.has_value());
  REQUIRE(result.value().empty());
  // Cleanup
  std::filesystem::remove_all(temp_dir);
}

TEST_CASE(
    "FlatbuffersTestDataLoader::LoadTestDataFbs loads test data from valid "
    "flatbuffers files",
    "[FlatbuffersTestDataLoader]") {
  // Arrange
  std::filesystem::path obj_dir_path(__FILE__);
  FlatbuffersTestDataLoader loader(obj_dir_path.parent_path());

  // Act
  auto result = loader.LoadTestDataFbs();
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  auto test_data_vector = result.value();
  
  // Assert - Basic structure
  REQUIRE(!test_data_vector.empty());
  REQUIRE(test_data_vector.size() >= 1);
  for (const auto &test_data : test_data_vector) {
    REQUIRE(test_data != nullptr);
  }
  
  const steamrot::TestDataFbs &first_test_data = *test_data_vector[0];
  
  // Assert - Metadata
  REQUIRE(first_test_data.meta_data() != nullptr);
  const steamrot::TestMetadataFbs *first_meta_data =
      first_test_data.meta_data();
  REQUIRE(first_meta_data->test_name()->str() ==
          "Test loading json to TestDataFbs");
  REQUIRE(first_meta_data->test_description()->str() ==
          "Validates that all fields in TestDataFbs can be loaded from JSON "
          "including engine snapshots");
  REQUIRE(first_meta_data->tags() != nullptr);
  REQUIRE(first_meta_data->tags()->size() == 3);
  REQUIRE(first_meta_data->will_pass() == true);
  REQUIRE(first_meta_data->version() == 1);

  // Assert - Simulation data
  const steamrot::SimulationDataFbs *first_sim_data =
      first_test_data.simulation_data();
  REQUIRE(first_sim_data != nullptr);
  REQUIRE(first_sim_data->description()->str() ==
          "This is a test simulation data for unit testing.");
  REQUIRE(first_sim_data->steps()->size() == 1);
  REQUIRE(first_sim_data->steps()->Get(0)->function_type() ==
          steamrot::FunctionEnumFbs_ProcessUIActionsAndEvents);
  
  // Assert - num_ticks
  REQUIRE(first_test_data.num_ticks() == 37);
  
  // Assert - starting_engine_state
  REQUIRE(first_test_data.starting_engine_state() != nullptr);
  const steamrot::EngineStateFbs *engine_state =
      first_test_data.starting_engine_state();
  REQUIRE(engine_state->running() == true);
  REQUIRE(engine_state->paused() == false);
  REQUIRE(engine_state->quit_requested() == false);
  
  // Assert - starting_engine_snapshot
  REQUIRE(first_test_data.starting_engine_snapshot() != nullptr);
  const steamrot::EngineSnapshotFbs *starting_snapshot =
      first_test_data.starting_engine_snapshot();
  REQUIRE(starting_snapshot->tick_number() == 0);
  
  // Assert - expected_engine_snapshots
  REQUIRE(first_test_data.expected_engine_snapshots() != nullptr);
  const auto *expected_snapshots = first_test_data.expected_engine_snapshots();
  REQUIRE(expected_snapshots->size() == 2);
  
  // Check first expected snapshot (tick 10)
  const auto *first_expected = expected_snapshots->Get(0);
  REQUIRE(first_expected->tick() == 10);
  REQUIRE(first_expected->snapshot() != nullptr);
  REQUIRE(first_expected->snapshot()->tick_number() == 10);
  
  // Check second expected snapshot (tick 37)
  const auto *second_expected = expected_snapshots->Get(1);
  REQUIRE(second_expected->tick() == 37);
  REQUIRE(second_expected->snapshot() != nullptr);
  REQUIRE(second_expected->snapshot()->tick_number() == 37);
}
