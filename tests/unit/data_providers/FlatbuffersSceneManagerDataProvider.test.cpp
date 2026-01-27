/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSceneManagerDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneManagerDataProvider.h"
#include "FailInfo.h"
#include "events_generated.h"
#include <catch2/catch_test_macros.hpp>

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
  REQUIRE(state.subscriptions[0]->m_trigger_event_type ==
          steamrot::EventType::CHANGE_SCENE);
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
  REQUIRE(data.scene_manager_state.subscriptions[0]->m_trigger_event_type ==
          steamrot::EventType::CHANGE_SCENE);
}
