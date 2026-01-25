/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the harness runner functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "harness_runner.h"
#include <catch2/catch_test_macros.hpp>
#include <format>

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
