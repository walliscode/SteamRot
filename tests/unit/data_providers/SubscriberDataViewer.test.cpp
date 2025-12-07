/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SubscriberDataViewer
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberDataViewer.h"
#include "subscriber_config_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>

TEST_CASE("SubscriberDataViewer with null pointer returns empty vector",
          "[unit][data_providers][SubscriberDataViewer]") {
  steamrot::SubscriberDataViewer viewer(nullptr);

  auto result = viewer.GetSubscriberConfigs();
  REQUIRE(result.has_value());
  REQUIRE(result.value().empty());
}

TEST_CASE("SubscriberDataViewer with empty vector returns empty result",
          "[unit][data_providers][SubscriberDataViewer]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create empty vector
  std::vector<flatbuffers::Offset<steamrot::SubscriberConfigFbs>> configs_vec;
  auto configs_offset = builder.CreateVector(configs_vec);

  // Build a minimal table to hold the vector (needed for testing)
  builder.Finish(configs_offset);

  // Get the vector from the buffer
  const auto *configs =
      flatbuffers::GetRoot<flatbuffers::Vector<
          flatbuffers::Offset<steamrot::SubscriberConfigFbs>>>(
          builder.GetBufferPointer());

  steamrot::SubscriberDataViewer viewer(configs);

  auto result = viewer.GetSubscriberConfigs();
  REQUIRE(result.has_value());
  REQUIRE(result.value().empty());
}

TEST_CASE("SubscriberDataViewer converts single subscriber config",
          "[unit][data_providers][SubscriberDataViewer]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create a single subscriber config
  auto config = steamrot::CreateSubscriberConfigFbs(
      builder, steamrot::EventType::EventType_EVENT_USER_INPUT,
      steamrot::EventDataData::EventDataData_NONE, 0, true);

  std::vector<flatbuffers::Offset<steamrot::SubscriberConfigFbs>> configs_vec;
  configs_vec.push_back(config);
  auto configs_offset = builder.CreateVector(configs_vec);

  builder.Finish(configs_offset);

  const auto *configs =
      flatbuffers::GetRoot<flatbuffers::Vector<
          flatbuffers::Offset<steamrot::SubscriberConfigFbs>>>(
          builder.GetBufferPointer());

  steamrot::SubscriberDataViewer viewer(configs);

  auto result = viewer.GetSubscriberConfigs();
  REQUIRE(result.has_value());
  REQUIRE(result.value().size() == 1);

  const auto &config_result = result.value()[0];
  REQUIRE(config_result.trigger_event_type ==
          steamrot::EventType::EventType_EVENT_USER_INPUT);
  REQUIRE(config_result.active == true);
  REQUIRE(!config_result.trigger_event_data.has_value());
}

TEST_CASE("SubscriberDataViewer converts multiple subscriber configs",
          "[unit][data_providers][SubscriberDataViewer]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create multiple subscriber configs
  auto config1 = steamrot::CreateSubscriberConfigFbs(
      builder, steamrot::EventType::EventType_EVENT_USER_INPUT,
      steamrot::EventDataData::EventDataData_NONE, 0, true);

  auto config2 = steamrot::CreateSubscriberConfigFbs(
      builder, steamrot::EventType::EventType_EVENT_TOGGLE_UI,
      steamrot::EventDataData::EventDataData_NONE, 0, false);

  std::vector<flatbuffers::Offset<steamrot::SubscriberConfigFbs>> configs_vec;
  configs_vec.push_back(config1);
  configs_vec.push_back(config2);
  auto configs_offset = builder.CreateVector(configs_vec);

  builder.Finish(configs_offset);

  const auto *configs =
      flatbuffers::GetRoot<flatbuffers::Vector<
          flatbuffers::Offset<steamrot::SubscriberConfigFbs>>>(
          builder.GetBufferPointer());

  steamrot::SubscriberDataViewer viewer(configs);

  auto result = viewer.GetSubscriberConfigs();
  REQUIRE(result.has_value());
  REQUIRE(result.value().size() == 2);

  const auto &config1_result = result.value()[0];
  REQUIRE(config1_result.trigger_event_type ==
          steamrot::EventType::EventType_EVENT_USER_INPUT);
  REQUIRE(config1_result.active == true);

  const auto &config2_result = result.value()[1];
  REQUIRE(config2_result.trigger_event_type ==
          steamrot::EventType::EventType_EVENT_TOGGLE_UI);
  REQUIRE(config2_result.active == false);
}
