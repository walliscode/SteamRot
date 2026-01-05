/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSceneManagerDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneManagerDataProvider.h"
#include "FailInfo.h"
#include "FlatbuffersDataLoader.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersSceneManagerDataProvider is constructed correctly",
          "[unit][FlatbuffersSceneManagerDataProvider]") {

  steamrot::FlatbuffersSceneManagerDataProvider provider;
  REQUIRE_NOTHROW(provider);
}

TEST_CASE("FlatbuffersSceneManagerDataProvider::ConfigureSceneManagerState "
          "handles nullptr"
          "[unit][FlatbuffersSceneManagerDataProvider]") {
  // Arrange
  steamrot::FlatbuffersSceneManagerDataProvider provider;
  steamrot::SceneManagerState state;

  // Act
  auto result = provider.ConfigureSceneManagerState(state, nullptr);

  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "SceneManagerStateFbs data is null");
}

TEST_CASE("FlatbuffersSceneManagerDataProvider::ConfigureSceneManagerState "
          "succeeds with valid data"
          "[unit][FlatbuffersSceneManagerDataProvider]") {
  // Arrange
  steamrot::FlatbuffersSceneManagerDataProvider provider;
  steamrot::SceneManagerState state;

  flatbuffers::FlatBufferBuilder builder;

  // create a SubscriberFbs to add

  auto subscriber_offset = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EventType_EVENT_TEST,
      steamrot::EventDataData::EventDataData_NONE,
      0,      // trigger_data_type and data
      false); // active

  auto state_data_offset = steamrot::CreateSceneManagerStateFbs(
      builder, builder.CreateVector(&subscriber_offset, 1));

  builder.Finish(state_data_offset);
  const steamrot::SceneManagerStateFbs *state_data =
      flatbuffers::GetRoot<steamrot::SceneManagerStateFbs>(
          builder.GetBufferPointer());

  // Act
  auto result = provider.ConfigureSceneManagerState(state, state_data);
  // Assert
  REQUIRE(result.has_value());
  REQUIRE(state.subscriptions.size() == 1); // No actual Subscribers created
  REQUIRE(state.subscriptions[0] != nullptr);
  REQUIRE(state.subscriptions[0]->m_trigger_event_type ==
          steamrot::EventType::EventType_EVENT_TEST);
}

TEST_CASE("FlatbuffersSceneManagerDataProvider::ConvertSceneManagerData "
          "returns error if null pointer",
          "[unit][FlatbuffersSceneManagerDataProvider]") {
  steamrot::FlatbuffersSceneManagerDataProvider provider;
  auto convert_result = provider.ConvertSceneManagerData(nullptr);
  REQUIRE(!convert_result.has_value());
  REQUIRE(convert_result.error().mode ==
          steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("FlatbuffersSceneManagerDataProvider::ConvertSceneManagerData "
          "converts FlatBuffers data to native SceneManagerData",
          "[unit][FlatbuffersSceneManagerDataProvider]") {
  steamrot::FlatbuffersSceneManagerDataProvider provider;
  steamrot::FlatbuffersDataLoader loader;

  // Load FlatBuffers data
  auto load_result = loader.ProvideSceneManagerData();
  REQUIRE(load_result.has_value());
  const steamrot::SceneManagerDataFbs *fb_data = load_result.value();

  // Convert using the new method
  auto convert_result = provider.ConvertSceneManagerData(fb_data);
  REQUIRE(convert_result.has_value());

  const auto &data = convert_result.value();
  // Check that data was populated correctly
  REQUIRE(data.scene_manager_state.subscriptions.size() == 1);
}

TEST_CASE("FlatbuffersSceneManagerDataProvider::ProvideSceneManagerData "
          "succeeds and provides correct data"
          "[unit][FlatbuffersSceneManagerDataProvider]") {
  // Arrange
  steamrot::FlatbuffersSceneManagerDataProvider provider;
  // Act
  auto result = provider.ProvideSceneManagerData();
  // Assert
  if (!result.has_value())
    FAIL("ProvideSceneManagerData failed: " + result.error().message);

  const steamrot::SceneManagerData &data = result.value();
  REQUIRE(data.scene_manager_state.subscriptions.size() == 1);
  REQUIRE(data.scene_manager_state.subscriptions[0] != nullptr);
  REQUIRE(data.scene_manager_state.subscriptions[0]->m_trigger_event_type ==
          steamrot::EventType::EventType_EVENT_CHANGE_SCENE);
}
