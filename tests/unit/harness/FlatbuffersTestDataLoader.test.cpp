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
  // Assert
  REQUIRE(!test_data_vector.empty());
  for (const auto &test_data : test_data_vector) {
    REQUIRE(test_data != nullptr);
  }
}
