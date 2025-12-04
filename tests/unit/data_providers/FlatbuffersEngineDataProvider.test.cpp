/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEngineDataProvider
/////////////////////////////////////////////////

#include "FlatbuffersEngineDataProvider.h"
#include "PathProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersEngineDataProvider loads engine core data",
          "[unit][data_providers][FlatbuffersEngineDataProvider]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersEngineDataProvider provider;

  auto result = provider.LoadEngineCoreData();
  REQUIRE(result.has_value());

  const auto &data = result.value();
  REQUIRE(data.window_width > 0);
  REQUIRE(data.window_height > 0);
  REQUIRE(!data.window_title.empty());
  REQUIRE(data.framerate_limit > 0);
}

TEST_CASE("FlatbuffersEngineDataProvider returns native struct",
          "[unit][data_providers][FlatbuffersEngineDataProvider]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersEngineDataProvider provider;

  auto result = provider.LoadEngineCoreData();
  REQUIRE(result.has_value());

  // Should be able to mutate the result
  auto data = result.value();
  data.window_width = 1920;
  data.window_title = "Modified";
  REQUIRE(data.window_width == 1920);
  REQUIRE(data.window_title == "Modified");
}

TEST_CASE("FlatbuffersEngineDataProvider loads complete engine data",
          "[unit][data_providers][FlatbuffersEngineDataProvider]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersEngineDataProvider provider;

  auto result = provider.LoadEngineData();
  REQUIRE(result.has_value());

  const auto &data = result.value();
  REQUIRE(data.core.window_width > 0);
  REQUIRE(data.core.window_height > 0);
}
