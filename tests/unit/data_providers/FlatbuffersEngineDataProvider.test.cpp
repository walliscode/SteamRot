/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEngineDataProvider
/////////////////////////////////////////////////

#include "FlatbuffersEngineDataProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersEngineDataProvider loads engine core data",
          "[unit][data_providers][FlatbuffersEngineDataProvider]") {
  steamrot::FlatbuffersEngineDataProvider provider;

  auto result = provider.LoadEngineCoreData();
  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &data = result.value();
  REQUIRE(data.window_width > 0);
  REQUIRE(data.window_height > 0);
  REQUIRE(!data.window_title.empty());
  REQUIRE(data.framerate_limit > 0);
}

TEST_CASE("FlatbuffersEngineDataProvider returns native struct",
          "[unit][data_providers][FlatbuffersEngineDataProvider]") {
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
  steamrot::FlatbuffersEngineDataProvider provider;

  auto result = provider.LoadEngineData();
  REQUIRE(result.has_value());

  const auto &data = result.value();
  REQUIRE(data.core.window_width > 0);
  REQUIRE(data.core.window_height > 0);
}

TEST_CASE("FlatbuffersEngineDataProvider provides SubscriberDataViewer",
          "[unit][data_providers][FlatbuffersEngineDataProvider]") {
  steamrot::FlatbuffersEngineDataProvider provider;

  // Should be able to get viewer from the provider
  auto viewer_result = provider.GetSubscriberViewer();
  REQUIRE(viewer_result.has_value());
  
  // Should be able to call GetSubscriberConfigs through the viewer
  const auto& viewer = viewer_result.value().get();
  auto configs_result = viewer.GetSubscriberConfigs();
  REQUIRE(configs_result.has_value());
  
  // Convenience method should also work
  auto result = provider.GetSubscriberConfigs();
  REQUIRE(result.has_value());
}
