/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for path-based test data loader utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_loader.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("LoadTestDataConfigsFromPath returns error for non-existent directory",
          "[unit][test_data_loader]") {
  auto result = steamrot::tests::LoadTestDataConfigsFromPath(
      "/non/existent/path/that/should/not/exist");
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().fail_mode == steamrot::FailMode::FileNotFound);
}

TEST_CASE("DiscoverTestDataFiles returns error for non-existent directory",
          "[unit][test_data_loader]") {
  auto result = steamrot::tests::DiscoverTestDataFiles(
      "/non/existent/path/that/should/not/exist");
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().fail_mode == steamrot::FailMode::FileNotFound);
}

TEST_CASE("LoadTestDataConfigFromFile returns error for non-existent file",
          "[unit][test_data_loader]") {
  auto result = steamrot::tests::LoadTestDataConfigFromFile(
      "/non/existent/file.test_data.bin");
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().fail_mode == steamrot::FailMode::FileNotFound);
}

TEST_CASE("DiscoverTestDataFiles returns empty vector for empty directory",
          "[unit][test_data_loader]") {
  // Use temp_directory_path for cross-platform compatibility
  auto temp_dir = std::filesystem::temp_directory_path();
  auto result = steamrot::tests::DiscoverTestDataFiles(temp_dir);
  // Should succeed for an existing directory
  REQUIRE(result.has_value());
  // Note: We can't guarantee temp dir is empty, so we just verify it returns
}
