/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for Subscriber configuration functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_subscribers.h"
#include "Subscriber.h"
#include "event_packet_generated.h"
#include "subscriber_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <variant>

/////////////////////////////////////////////////
// CreateSubscriber tests
/////////////////////////////////////////////////

TEST_CASE("CreateSubscriber fails with null pointer", "[unit][configure_subscribers]") {
  auto result = steamrot::data::configure::CreateSubscriber(nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("CreateSubscriber fails with EventType NONE",
          "[unit][configure_subscribers]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SubscriberFbs with EventType_NONE
  auto subscriber_offset =
      steamrot::CreateSubscriberFbs(builder, false, steamrot::EventTypeFbs_NONE);
  builder.Finish(subscriber_offset);

  const steamrot::SubscriberFbs *subscriber_fbs =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());

  auto result = steamrot::data::configure::CreateSubscriber(subscriber_fbs);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::EnumValueNotHandled);
}

TEST_CASE("CreateSubscriber creates subscriber without trigger data",
          "[unit][configure_subscribers]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SubscriberFbs with just event type, no trigger data
  auto subscriber_offset =
      steamrot::CreateSubscriberFbs(builder, false, // active
                                    steamrot::EventTypeFbs_UI_TOGGLE);
  builder.Finish(subscriber_offset);

  const steamrot::SubscriberFbs *subscriber_fbs =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());

  auto result = steamrot::data::configure::CreateSubscriber(subscriber_fbs);

  REQUIRE(result.has_value());
  REQUIRE(result.value().m_trigger_event_type == steamrot::EventType::UI_TOGGLE);
  REQUIRE_FALSE(result.value().m_active);
  REQUIRE_FALSE(result.value().m_trigger_event_data.has_value());
}

TEST_CASE("CreateSubscriber creates active subscriber without trigger data",
          "[unit][configure_subscribers]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SubscriberFbs with active flag set
  auto subscriber_offset =
      steamrot::CreateSubscriberFbs(builder, true, // active
                                    steamrot::EventTypeFbs_SCENE_CHANGE);
  builder.Finish(subscriber_offset);

  const steamrot::SubscriberFbs *subscriber_fbs =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());

  auto result = steamrot::data::configure::CreateSubscriber(subscriber_fbs);

  REQUIRE(result.has_value());
  REQUIRE(result.value().m_trigger_event_type == steamrot::EventType::SCENE_CHANGE);
  REQUIRE(result.value().m_active);
  REQUIRE_FALSE(result.value().m_trigger_event_data.has_value());
}

TEST_CASE("ConfigureSubscriber populates subscriber fields correctly",
          "[unit][configure_subscribers]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SubscriberFbs
  auto subscriber_offset =
      steamrot::CreateSubscriberFbs(builder, true, // active
                                    steamrot::EventTypeFbs_LOGIC_TOGGLE);
  builder.Finish(subscriber_offset);

  const steamrot::SubscriberFbs *subscriber_fbs =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());

  steamrot::Subscriber subscriber;
  auto result = steamrot::data::configure::ConfigureSubscriber(subscriber, subscriber_fbs);

  REQUIRE(result.has_value());
  REQUIRE(subscriber.m_active == true);
  REQUIRE(subscriber.m_trigger_event_type == steamrot::EventType::LOGIC_TOGGLE);
}
