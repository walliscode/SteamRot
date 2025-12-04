/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSceneDataProvider
/////////////////////////////////////////////////

#include "FlatbuffersSceneDataProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersSceneDataProvider loads scene core data",
          "[unit][data_providers][FlatbuffersSceneDataProvider]") {
  steamrot::FlatbuffersSceneDataProvider provider;

  auto result = provider.LoadSceneCoreData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(result.has_value());

  const auto &data = result.value();
  REQUIRE(data.render_texture_width > 0);
  REQUIRE(data.render_texture_height > 0);
}

TEST_CASE("FlatbuffersSceneDataProvider returns mutable native struct",
          "[unit][data_providers][FlatbuffersSceneDataProvider]") {
  steamrot::FlatbuffersSceneDataProvider provider;

  auto result = provider.LoadSceneCoreData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(result.has_value());

  // Should be able to mutate the result
  auto data = result.value();
  data.render_texture_width = 1920;
  data.render_texture_height = 1080;
  REQUIRE(data.render_texture_width == 1920);
  REQUIRE(data.render_texture_height == 1080);
}

TEST_CASE("FlatbuffersSceneDataProvider loads complete scene data",
          "[unit][data_providers][FlatbuffersSceneDataProvider]") {
  steamrot::FlatbuffersSceneDataProvider provider;

  auto result = provider.LoadSceneData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(result.has_value());

  const auto &data = result.value();
  REQUIRE(data.scene_type == steamrot::SceneType::SceneType_TEST);
  REQUIRE(data.core.render_texture_width > 0);
  REQUIRE(data.core.render_texture_height > 0);
}
