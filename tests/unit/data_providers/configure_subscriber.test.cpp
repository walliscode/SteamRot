/////////////////////////////////////////////////
/// @file
/// @brief unit tests for configure_subscriber free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_subscriber.h"
#include "event_packet_generated.h"
#include "event_payload_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConfigureSubscriber returns error when given null data",
          "[ConfigureSubscriber]") {
  // arrange
  steamrot::Subscriber subscriber;
  // act
  auto result =
      steamrot::data::configure::ConfigureSubscriber(subscriber, nullptr);
  // assert
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "SubscriberFbs data is null, cannot populate Subscriber");
}

TEST_CASE("ConfigureSubscriber sets active to false by default",
          "[ConfigureSubscriber]") {
  // arrange
  steamrot::Subscriber subscriber;
  subscriber.m_active = true; // set to true to ensure it gets set to false

  flatbuffers::FlatBufferBuilder builder;
  auto system_payload =
      steamrot::CreateSystemPayloadFbs(builder, steamrot::SystemActionFbs_QUIT);
  auto fb_subscriber = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventTypeFbs_SYSTEM,
      steamrot::EventPayloadFbs_SystemPayloadFbs, system_payload.Union());
  builder.Finish(fb_subscriber);

  auto fb_subscriber_ptr =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());
  // act
  auto result = steamrot::data::configure::ConfigureSubscriber(
      subscriber, fb_subscriber_ptr);
  // assert
  REQUIRE(result.has_value());
  REQUIRE_FALSE(subscriber.m_active);
}

TEST_CASE("EventType is configured and not None when valid data is provided",
          "[ConfigureSubscriber]") {
  // arrange
  steamrot::Subscriber subscriber;
  REQUIRE(subscriber.event_type == steamrot::EventType::NONE);

  flatbuffers::FlatBufferBuilder builder;
  auto system_payload =
      steamrot::CreateSystemPayloadFbs(builder, steamrot::SystemActionFbs_QUIT);
  auto fb_subscriber = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventTypeFbs_SYSTEM,
      steamrot::EventPayloadFbs_SystemPayloadFbs, system_payload.Union());
  builder.Finish(fb_subscriber);
  auto fb_subscriber_ptr =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());
  // act
  auto result = steamrot::data::configure::ConfigureSubscriber(
      subscriber, fb_subscriber_ptr);

  // assert
  if (!result.has_value()) {
    FAIL("ConfigureSubscriber failed with error: " + result.error().message);
  }
  REQUIRE(subscriber.event_type != steamrot::EventType::NONE);
  REQUIRE(subscriber.event_type == steamrot::EventType::SYSTEM);
}

TEST_CASE("Subscriber filter_payload is configured correctly when valid data "
          "is provided",
          "[ConfigureSubscriber]") {
  // arrange
  steamrot::Subscriber subscriber;
  REQUIRE(std::holds_alternative<std::monostate>(subscriber.filter_payload));
  flatbuffers::FlatBufferBuilder builder;

  SECTION("SystemPayload is configured correctly") {
    auto system_payload = steamrot::CreateSystemPayloadFbs(
        builder, steamrot::SystemActionFbs_QUIT);
    auto fb_subscriber = steamrot::CreateSubscriberFbs(
        builder, steamrot::EventTypeFbs_SYSTEM,
        steamrot::EventPayloadFbs_SystemPayloadFbs, system_payload.Union());
    builder.Finish(fb_subscriber);
    auto fb_subscriber_ptr = flatbuffers::GetRoot<steamrot::SubscriberFbs>(
        builder.GetBufferPointer());
    // act
    auto result = steamrot::data::configure::ConfigureSubscriber(
        subscriber, fb_subscriber_ptr);
    // assert
    if (!result.has_value()) {
      FAIL("ConfigureSubscriber failed with error: " + result.error().message);
    }
    REQUIRE(std::holds_alternative<steamrot::SystemPayload>(
        subscriber.filter_payload));
    REQUIRE(
        std::get<steamrot::SystemPayload>(subscriber.filter_payload).action ==
        steamrot::SystemPayload::SystemAction::QUIT);
  }

  SECTION("InputPayload is configured correctly") {
    auto input_payload = steamrot::CreateInputPayloadFbs(
        builder, steamrot::InputActionFbs_SELECT);
    auto fb_subscriber = steamrot::CreateSubscriberFbs(
        builder, steamrot::EventTypeFbs_USER_INPUT,
        steamrot::EventPayloadFbs_InputPayloadFbs, input_payload.Union());
    builder.Finish(fb_subscriber);
    auto fb_subscriber_ptr = flatbuffers::GetRoot<steamrot::SubscriberFbs>(
        builder.GetBufferPointer());
    // act
    auto result = steamrot::data::configure::ConfigureSubscriber(
        subscriber, fb_subscriber_ptr);
    // assert
    if (!result.has_value()) {
      FAIL("ConfigureSubscriber failed with error: " + result.error().message);
    }
    REQUIRE(std::holds_alternative<steamrot::InputPayload>(
        subscriber.filter_payload));
    REQUIRE(
        std::get<steamrot::InputPayload>(subscriber.filter_payload).action ==
        steamrot::InputPayload::InputAction::SELECT);
  }
}
