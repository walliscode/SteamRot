/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSubscriberViewer
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FlatbuffersSubscriberViewer.h"
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>

TEST_CASE("FlatbuffersSubscriberViewer with null pointer returns empty vector",
          "[unit][data_providers][FlatbuffersSubscriberViewer]") {
  steamrot::FlatbuffersSubscriberViewer viewer(nullptr);

  auto result = viewer.GetSubscribers();
  REQUIRE(result.has_value());
  REQUIRE(result.value().empty());
}

TEST_CASE("FlatbuffersSubscriberViewer with empty vector returns empty result",
          "[unit][data_providers][FlatbuffersSubscriberViewer]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create empty vector
  std::vector<flatbuffers::Offset<steamrot::SubscriberFbs>> subscribers_vec;
  auto subscribers_offset = builder.CreateVector(subscribers_vec);

  // Build a minimal table to hold the vector (needed for testing)
  builder.Finish(subscribers_offset);

  // Get the vector from the buffer
  const auto *subscribers = flatbuffers::GetRoot<
      flatbuffers::Vector<flatbuffers::Offset<steamrot::SubscriberFbs>>>(
      builder.GetBufferPointer());

  steamrot::FlatbuffersSubscriberViewer viewer(subscribers);

  auto result = viewer.GetSubscribers();
  REQUIRE(result.has_value());
  REQUIRE(result.value().empty());
}

TEST_CASE("FlatbuffersSubscriberViewer converts single subscriber",
          "[unit][data_providers][FlatbuffersSubscriberViewer]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create a single subscriber
  auto subscriber = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EventType_EVENT_USER_INPUT,
      steamrot::EventDataData::EventDataData_NONE, 0, true);

  std::vector<flatbuffers::Offset<steamrot::SubscriberFbs>> subscribers_vec;
  subscribers_vec.push_back(subscriber);
  auto subscribers_offset = builder.CreateVector(subscribers_vec);

  builder.Finish(subscribers_offset);

  const auto *subscribers = flatbuffers::GetRoot<
      flatbuffers::Vector<flatbuffers::Offset<steamrot::SubscriberFbs>>>(
      builder.GetBufferPointer());

  steamrot::FlatbuffersSubscriberViewer viewer(subscribers);

  auto result = viewer.GetSubscribers();
  REQUIRE(result.has_value());
  REQUIRE(result.value().size() == 1);

  const auto &subscriber_result = result.value()[0];
  REQUIRE(subscriber_result->m_trigger_event_type ==
          steamrot::EventType::EventType_EVENT_USER_INPUT);
  REQUIRE(subscriber_result->m_active == true);
  REQUIRE(!subscriber_result->m_trigger_event_data.has_value());
}

TEST_CASE("FlatbuffersSubscriberViewer converts multiple subscribers",
          "[unit][data_providers][FlatbuffersSubscriberViewer]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create multiple subscribers
  auto subscriber1 = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EventType_EVENT_USER_INPUT,
      steamrot::EventDataData::EventDataData_NONE, 0, true);

  auto subscriber2 = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EventType_EVENT_TOGGLE_UI,
      steamrot::EventDataData::EventDataData_NONE, 0, false);

  std::vector<flatbuffers::Offset<steamrot::SubscriberFbs>> subscribers_vec;
  subscribers_vec.push_back(subscriber1);
  subscribers_vec.push_back(subscriber2);
  auto subscribers_offset = builder.CreateVector(subscribers_vec);

  builder.Finish(subscribers_offset);

  const auto *subscribers = flatbuffers::GetRoot<
      flatbuffers::Vector<flatbuffers::Offset<steamrot::SubscriberFbs>>>(
      builder.GetBufferPointer());

  steamrot::FlatbuffersSubscriberViewer viewer(subscribers);

  auto result = viewer.GetSubscribers();
  REQUIRE(result.has_value());
  REQUIRE(result.value().size() == 2);

  const auto &subscriber1_result = result.value()[0];
  REQUIRE(subscriber1_result->m_trigger_event_type ==
          steamrot::EventType::EventType_EVENT_USER_INPUT);
  REQUIRE(subscriber1_result->m_active == true);

  const auto &subscriber2_result = result.value()[1];
  REQUIRE(subscriber2_result->m_trigger_event_type ==
          steamrot::EventType::EventType_EVENT_TOGGLE_UI);
  REQUIRE(subscriber2_result->m_active == false);
}
