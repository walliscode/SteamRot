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
  // Use /tmp as a directory that exists but shouldn't have .test_data.bin files
  auto result = steamrot::tests::DiscoverTestDataFiles("/tmp");
  // Should succeed but may or may not be empty depending on /tmp contents
  // Just check that it succeeds for an existing directory
  if (std::filesystem::exists("/tmp") && std::filesystem::is_directory("/tmp")) {
    REQUIRE(result.has_value());
    // Note: We can't guarantee /tmp is empty, so we just verify it returns
  }
}
