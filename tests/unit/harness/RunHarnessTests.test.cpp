/////////////////////////////////////////////////
/// @file
/// @brief Integration test for RunHarnessTests function.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "harness_runner.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("RunHarnessTests executes successfully with valid test data",
          "[unit][harness_runner][integration]") {
  // Arrange
  // Use the current test file's location to find the adjacent data directory
  std::filesystem::path current_file = __FILE__;
  std::filesystem::path test_dir = current_file.parent_path();

  // Act
  auto result = steamrot::tests::RunHarnessTests(test_dir);

  // Assert
  if (!result.has_value()) {
    FAIL("RunHarnessTests failed: " + result.error().message);
  }
  REQUIRE(result.has_value());
}

TEST_CASE("RunHarnessTests processes all test data files in data directory",
          "[unit][harness_runner][integration]") {
  // Arrange
  std::filesystem::path current_file = __FILE__;
  std::filesystem::path test_dir = current_file.parent_path();

  // Verify data directory exists
  std::filesystem::path data_dir = test_dir / "data";
  REQUIRE(std::filesystem::exists(data_dir));
  REQUIRE(std::filesystem::is_directory(data_dir));

  // Count .test_data.bin files
  size_t test_file_count = 0;
  for (const auto &entry : std::filesystem::directory_iterator(data_dir)) {
    if (entry.path().extension() == ".bin" &&
        entry.path().filename().string().find(".test_data.") !=
            std::string::npos) {
      test_file_count++;
    }
  }

  // Require at least one test file
  REQUIRE(test_file_count > 0);

  // Act
  auto result = steamrot::tests::RunHarnessTests(test_dir);

  // Assert
  if (!result.has_value()) {
    FAIL("RunHarnessTests failed: " + result.error().message);
  }
}
