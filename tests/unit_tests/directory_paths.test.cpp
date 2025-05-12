#include "steamrot_directory_paths.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("Check directory paths") {

  // check the data folder exists
  REQUIRE(std::filesystem::exists(getDataFolder()));

  // check the data/scene folder exists
  REQUIRE(std::filesystem::exists(getSceneFolder()));

  // check the data/logs folder exists
  REQUIRE(std::filesystem::exists(getLogsFolder()));

  // check the tests folder exists
  REQUIRE(std::filesystem::exists(getTestFolder()));

  // check the tests/data folder exists
  REQUIRE(std::filesystem::exists(getTestDataFolder()));
}
