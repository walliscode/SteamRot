/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the FlatbuffersTestDataLoader class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersTestDataLoader.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersTestDataLoader constructor sets object directory path "
          "correctly",
          "[unit][FlatbuffersTestDataLoader]") {

  // Arrange
  std::filesystem::path obj_dir_path =
      std::filesystem::path(__FILE__).parent_path();

  // Act
  steamrot::FlatbuffersTestDataLoader loader(obj_dir_path);

  // Assert
  REQUIRE(loader.GetObjectDirectoryPath() == obj_dir_path);
}

TEST_CASE("FlatbuffersTestDataLoader::GetAdjacentDataDirectoryPath returns "
          "the correct path when data directory exists",
          "[unit][FlatbuffersTestDataLoader]") {

  // Arrange
  std::filesystem::path obj_dir_path =
      std::filesystem::path(__FILE__).parent_path();
  steamrot::FlatbuffersTestDataLoader loader(obj_dir_path);

  // Act
  auto result = loader.GetAdjacentDataDirectoryPath();

  // Assert
  REQUIRE(result.has_value());
  REQUIRE(result.value() == obj_dir_path / "data");
}

TEST_CASE("FlatbuffersTestDataLoader::GetAdjacentDataDirectoryPath returns "
          "error when data directory does not exist",
          "[unit][FlatbuffersTestDataLoader]") {

  // Arrange
  std::filesystem::path obj_dir_path =
      std::filesystem::temp_directory_path() / "non_existent_directory";
  steamrot::FlatbuffersTestDataLoader loader(obj_dir_path);

  // Act
  auto result = loader.GetAdjacentDataDirectoryPath();

  // Assert
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::DirectoryNotFound);
}

TEST_CASE("FlatbuffersTestDataLoader::LoadTestDataFbs returns error when data "
          "directory does not exist",
          "[unit][FlatbuffersTestDataLoader]") {

  // Arrange
  std::filesystem::path obj_dir_path =
      std::filesystem::temp_directory_path() / "non_existent_directory";
  steamrot::FlatbuffersTestDataLoader loader(obj_dir_path);

  // Act
  auto result = loader.LoadTestDataFbs();

  // Assert
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::DirectoryNotFound);
}

TEST_CASE("FlatbuffersTestDataLoader::LoadTestDataFbs returns empty vector "
          "when data directory is empty",
          "[unit][FlatbuffersTestDataLoader]") {

  // Arrange
  std::filesystem::path temp_dir =
      std::filesystem::temp_directory_path() / "empty_test_data_directory";
  std::filesystem::create_directories(temp_dir / "data");
  steamrot::FlatbuffersTestDataLoader loader(temp_dir);

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
    "[unit][FlatbuffersTestDataLoader]") {

  // Arrange
  std::filesystem::path obj_dir_path =
      std::filesystem::path(__FILE__).parent_path();
  steamrot::FlatbuffersTestDataLoader loader(obj_dir_path);

  // Act
  auto result = loader.LoadTestDataFbs();

  // Assert
  if (!result.has_value())
    FAIL(result.error().message);

  const auto &test_data_vector = result.value();
  REQUIRE_FALSE(test_data_vector.empty());

  for (const auto *test_data : test_data_vector) {
    REQUIRE(test_data != nullptr);
  }

  const steamrot::TestDataFbs &first = *test_data_vector[0];
  REQUIRE(first.meta_data() != nullptr);
  REQUIRE(first.meta_data()->test_name() != nullptr);
  REQUIRE(first.meta_data()->test_name()->str() ==
          "FlatbuffersTestDataProvider unit test data");
  REQUIRE(first.simulation_data() != nullptr);
  REQUIRE(first.simulation_data()->steps() != nullptr);
  REQUIRE(first.simulation_data()->steps()->size() == 1);
  REQUIRE(first.num_ticks() == 3);
}
