/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSceneManagerDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneManagerDataProvider.h"
#include "events_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersSceneManagerDataProvider is constructed correctly",
          "[unit][FlatbuffersSceneManagerDataProvider]") {

  steamrot::FlatbuffersSceneManagerDataProvider provider;
  REQUIRE_NOTHROW(provider);
}

TEST_CASE("FlatbuffersSceneManagerDataProvider::LoadSceneManagerState loads "
          "correctly",
          "[unit][FlatbuffersSceneManagerDataProvider]") {

  steamrot::FlatbuffersSceneManagerDataProvider provider;
  auto result = provider.LoadSceneManagerState();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &state = result.value();

  REQUIRE(state.subscriptions.size() == 1);
  REQUIRE(state.subscriptions[0]->m_trigger_event_type ==
          steamrot::EventType_EVENT_CHANGE_SCENE);
}

TEST_CASE(
    "FlatbuffersSceneManagerDataProvider::GetSubscriberViewer returns viewer ",
    "[unit][FlatbuffersEngineDataProvider]") {

  steamrot::FlatbuffersSceneManagerDataProvider provider;
  auto result = provider.GetSubscriberViewer();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &viewer = result.value();
  REQUIRE(viewer != nullptr);

  // Test that viewer can get subscribers
  auto subscribers_result = viewer->GetSubscribers();
  if (!subscribers_result.has_value()) {
    FAIL(subscribers_result.error().message);
  }

  // check subscribers for size and specific data. the Engine should have a
  // specific set of subscribers
  REQUIRE(subscribers_result.value().size() == 1);
  REQUIRE(subscribers_result.value()[0]->m_trigger_event_type ==
          steamrot::EventType_EVENT_CHANGE_SCENE);
}

TEST_CASE("FlatbuffersSceneManagerDataProvider::LoadSceneManagerData loads "
          "correctly",
          "[unit][FlatbuffersSceneManagerDataProvider]") {

  steamrot::FlatbuffersSceneManagerDataProvider provider;
  auto result = provider.LoadSceneManagerData();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &data = result.value();

  // Check SceneManagerState
  const auto &state = data.scene_manager_state;
  REQUIRE(state.subscriptions.size() == 1);
  REQUIRE(state.subscriptions[0]->m_trigger_event_type ==
          steamrot::EventType_EVENT_CHANGE_SCENE);

  // Check SceneCollectionData (currently empty by default)
  REQUIRE(data.scene_collection_data.empty());
}
