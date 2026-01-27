/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for configure_scene_manager_data functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_scene_manager_data.h"
#include "FlatbuffersDataLoader.h"
#include "SceneManagerState.h"
#include "events_generated.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
// ConfigureSceneManagerState tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureSceneManagerState fails with null data",
          "[unit][configure_scene_manager_data]") {
  steamrot::SceneManagerState state;

  auto result =
      steamrot::data::configure::ConfigureSceneManagerState(state, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "SceneManagerStateFbs data is null");
}

TEST_CASE("ConfigureSceneManagerState populates from valid data",
          "[unit][configure_scene_manager_data]") {
  // Load real flatbuffers data
  steamrot::FlatbuffersDataLoader loader;
  auto scene_manager_data_result = loader.ProvideSceneManagerData();
  REQUIRE(scene_manager_data_result.has_value());

  const auto *scene_manager_data = scene_manager_data_result.value();
  const auto *state_data = scene_manager_data->state();
  REQUIRE(state_data != nullptr);

  steamrot::SceneManagerState state;
  auto result =
      steamrot::data::configure::ConfigureSceneManagerState(state, state_data);

  REQUIRE(result.has_value());
  REQUIRE(state.subscriptions.size() == 1);
  REQUIRE(state.subscriptions[0] != nullptr);
  REQUIRE(state.subscriptions[0]->m_trigger_event_type ==
          steamrot::EventType::EVENT_CHANGE_SCENE);
}

TEST_CASE("ConfigureSceneManagerState handles empty subscriptions",
          "[unit][configure_scene_manager_data]") {
  // Create a SceneManagerStateFbs with no subscriptions
  flatbuffers::FlatBufferBuilder builder;
  
  std::vector<flatbuffers::Offset<steamrot::SubscriberFbs>> empty_subscribers;
  auto state_data_offset = steamrot::CreateSceneManagerStateFbs(
      builder, builder.CreateVector(empty_subscribers));

  builder.Finish(state_data_offset);
  const steamrot::SceneManagerStateFbs *state_data =
      flatbuffers::GetRoot<steamrot::SceneManagerStateFbs>(
          builder.GetBufferPointer());

  steamrot::SceneManagerState state;
  auto result =
      steamrot::data::configure::ConfigureSceneManagerState(state, state_data);

  REQUIRE(result.has_value());
  REQUIRE(state.subscriptions.empty());
}

TEST_CASE("ConfigureSceneManagerState handles multiple subscriptions",
          "[unit][configure_scene_manager_data]") {
  // Create a SceneManagerStateFbs with multiple subscriptions
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<steamrot::SubscriberFbs>> subscribers;
  
  auto subscriber1 = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EVENT_CHANGE_SCENE,
      steamrot::EventDataData::EventDataData_NONE, 0, true);
  
  auto subscriber2 = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EVENT_QUIT_GAME,
      steamrot::EventDataData::EventDataData_NONE, 0, false);

  subscribers.push_back(subscriber1);
  subscribers.push_back(subscriber2);

  auto state_data_offset = steamrot::CreateSceneManagerStateFbs(
      builder, builder.CreateVector(subscribers));

  builder.Finish(state_data_offset);
  const steamrot::SceneManagerStateFbs *state_data =
      flatbuffers::GetRoot<steamrot::SceneManagerStateFbs>(
          builder.GetBufferPointer());

  steamrot::SceneManagerState state;
  auto result =
      steamrot::data::configure::ConfigureSceneManagerState(state, state_data);

  REQUIRE(result.has_value());
  REQUIRE(state.subscriptions.size() == 2);
  REQUIRE(state.subscriptions[0]->m_trigger_event_type ==
          steamrot::EventType::EVENT_CHANGE_SCENE);
  REQUIRE(state.subscriptions[0]->m_active == true);
  REQUIRE(state.subscriptions[1]->m_trigger_event_type ==
          steamrot::EventType::EVENT_QUIT_GAME);
  REQUIRE(state.subscriptions[1]->m_active == false);
}
