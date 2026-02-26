/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSceneManagerDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneManagerDataProvider.h"
#include "EventPayload.h"
#include "FailInfo.h"
#include <catch2/catch_test_macros.hpp>
#include <variant>

TEST_CASE("FlatbuffersSceneManagerDataProvider is constructed correctly",
          "[unit][FlatbuffersSceneManagerDataProvider]") {

  steamrot::FlatbuffersSceneManagerDataProvider provider;
  REQUIRE_NOTHROW(provider);
}

TEST_CASE("FlatbuffersSceneManagerDataProvider::CreateSceneManagerData loads "
          "correctly",
          "[unit][FlatbuffersSceneManagerDataProvider]") {
  steamrot::FlatbuffersSceneManagerDataProvider provider;
  auto result = provider.CreateSceneManagerData();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &data = result.value();

  // Check SceneManagerState
  const auto &state = data.scene_manager_state;
  REQUIRE(state.subscriptions.size() == 1);
  REQUIRE(state.subscriptions[0] != nullptr);

  REQUIRE(std::holds_alternative<steamrot::ScenePayload>(
      state.subscriptions[0]->filter_payload));
  steamrot::ScenePayload filter_payload =
      std::get<steamrot::ScenePayload>(state.subscriptions[0]->filter_payload);
  REQUIRE(filter_payload.action == steamrot::ScenePayload::SceneAction::CHANGE);
}

TEST_CASE("FlatbuffersSceneManagerDataProvider::ConfigureSceneManagerData "
          "configures correctly",
          "[unit][FlatbuffersSceneManagerDataProvider]") {
  steamrot::FlatbuffersSceneManagerDataProvider provider;
  steamrot::SceneManagerData data;

  auto result = provider.ConfigureSceneManagerData(data);

  REQUIRE(result.has_value());
  REQUIRE(data.scene_manager_state.subscriptions.size() == 1);
  REQUIRE(data.scene_manager_state.subscriptions[0] != nullptr);

  REQUIRE(std::holds_alternative<steamrot::ScenePayload>(
      data.scene_manager_state.subscriptions[0]->filter_payload));
  steamrot::ScenePayload filter_payload = std::get<steamrot::ScenePayload>(
      data.scene_manager_state.subscriptions[0]->filter_payload);
  REQUIRE(filter_payload.action == steamrot::ScenePayload::SceneAction::CHANGE);
}
