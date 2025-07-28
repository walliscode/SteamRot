/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersDataLoader
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Dataloader fails to initiate PathProvider", "[DataLoader]") {
  REQUIRE_THROWS(
      steamrot::FlatbuffersDataLoader{steamrot::EnvironmentType::None});
}

TEST_CASE("DataLoader can be initiated with a valid environment",
          "[DataLoader]") {
  REQUIRE_NOTHROW(
      steamrot::FlatbuffersDataLoader{steamrot::EnvironmentType::Test});
}

TEST_CASE("DataLoader can be called with a None environment after being set",
          "[DataLoader]") {
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  REQUIRE_NOTHROW(steamrot::FlatbuffersDataLoader{});
}

TEST_CASE("DataLoader can only initiate PathProvider once", "[DataLoader]") {
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  REQUIRE_THROWS(
      steamrot::FlatbuffersDataLoader{steamrot::EnvironmentType::Production});
}
