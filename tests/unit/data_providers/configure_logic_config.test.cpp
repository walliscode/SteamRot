/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the logic config configuration logic.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_logic_config.h"
#include "EventType.h"
#include "event_payload_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConfigureLogicConfig returns unexpected when given null pointer",
          "[ConfigureLogicConfig]") {
  // Arrange
  steamrot::LogicConfig logic_config;
  // Act
  auto result =
      steamrot::data::configure::ConfigureLogicConfig(logic_config, nullptr);
  // Assert
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().message == "LogicConfigFbs pointer is null.");
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureLogicConfig populates the LogicConfig subscribers",
          "[ConfigureLogicConfig]") {
  // Arrange
  steamrot::LogicConfig logic_config;

  flatbuffers::FlatBufferBuilder builder;

  // create SubscriberFbs objects
  std::vector<flatbuffers::Offset<steamrot::SubscriberFbs>> subscribers;
  auto subscriber1 = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventTypeFbs_SCENE,
      steamrot::EventPayloadFbs_ScenePayloadFbs, 0);

  auto subscriber2 = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventTypeFbs_SYSTEM,
      steamrot::EventPayloadFbs_SystemPayloadFbs, 0);

  subscribers.push_back(subscriber1);
  subscribers.push_back(subscriber2);

  // create LogicConfigFbs object
  auto logic_config_fbs_offset = steamrot::CreateLogicConfigFbs(
      builder, builder.CreateVector(subscribers));
  builder.Finish(logic_config_fbs_offset);
  auto logic_config_fbs = flatbuffers::GetRoot<steamrot::LogicConfigFbs>(
      builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureLogicConfig(
      logic_config, logic_config_fbs);

  // Assert
  REQUIRE(result.has_value());
  REQUIRE(logic_config.m_subscribers.size() == 2);
  auto subscriber1_ptr = logic_config.m_subscribers[0];
  auto subscriber2_ptr = logic_config.m_subscribers[1];
  REQUIRE(subscriber1_ptr->event_type == steamrot::EventType::SCENE);
  REQUIRE(subscriber2_ptr->event_type == steamrot::EventType::SYSTEM);
}
