/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the subscriber_factory namespace
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "subscriber_factory.h"
#include "Subscriber.h"
#include "events_generated.h"
#include "subscriber_generated.h"
#include "user_input_generated.h"
#include <SFML/Window/Keyboard.hpp>
#include <catch2/catch_test_macros.hpp>
#include <variant>

/////////////////////////////////////////////////
// CreateSubscriber tests
/////////////////////////////////////////////////

TEST_CASE("CreateSubscriber fails with null pointer", "[subscriber_factory]") {
  auto result = steamrot::subscriber_factory::CreateSubscriber(nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("CreateSubscriber fails with EventType NONE",
          "[subscriber_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SubscriberFbs with EventType_NONE
  auto subscriber_offset = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EventType_NONE);
  builder.Finish(subscriber_offset);

  const steamrot::SubscriberFbs *subscriber_fbs =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());

  auto result = steamrot::subscriber_factory::CreateSubscriber(subscriber_fbs);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::EnumValueNotHandled);
}

TEST_CASE("CreateSubscriber creates subscriber without trigger data",
          "[unit][subscriber_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SubscriberFbs with just event type, no trigger data
  auto subscriber_offset = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EventType_EVENT_TEST,
      steamrot::EventDataData::EventDataData_NONE,
      0,      // trigger_data_type and data
      false); // active
  builder.Finish(subscriber_offset);

  const steamrot::SubscriberFbs *subscriber_fbs =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());

  auto result = steamrot::subscriber_factory::CreateSubscriber(subscriber_fbs);

  REQUIRE(result.has_value());
  REQUIRE(result.value().m_trigger_event_type ==
          steamrot::EventType::EventType_EVENT_TEST);
  REQUIRE_FALSE(result.value().m_active);
  REQUIRE_FALSE(result.value().m_trigger_event_data.has_value());
}

TEST_CASE("CreateSubscriber creates active subscriber without trigger data",
          "[unit][subscriber_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SubscriberFbs with active flag set
  auto subscriber_offset = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EventType_EVENT_TEST,
      steamrot::EventDataData::EventDataData_NONE,
      0,     // trigger_data_type and data
      true); // active
  builder.Finish(subscriber_offset);

  const steamrot::SubscriberFbs *subscriber_fbs =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());

  auto result = steamrot::subscriber_factory::CreateSubscriber(subscriber_fbs);

  REQUIRE(result.has_value());
  REQUIRE(result.value().m_trigger_event_type ==
          steamrot::EventType::EventType_EVENT_TEST);
  REQUIRE(result.value().m_active);
  REQUIRE_FALSE(result.value().m_trigger_event_data.has_value());
}

TEST_CASE("CreateSubscriber creates subscriber with UserInputBitset trigger "
          "data",
          "[unit][subscriber_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create UserInputBitsetData with A key pressed
  std::vector<uint8_t> keyboard_pressed = {steamrot::KeyboardInput_A};
  auto keyboard_pressed_vec = builder.CreateVector(keyboard_pressed);
  auto trigger_data_offset =
      steamrot::CreateUserInputBitsetData(builder, keyboard_pressed_vec);

  // Create SubscriberFbs with trigger data
  auto subscriber_offset = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EventType_EVENT_USER_INPUT,
      steamrot::EventDataData::EventDataData_UserInputBitsetData,
      trigger_data_offset.Union(), false);
  builder.Finish(subscriber_offset);

  const steamrot::SubscriberFbs *subscriber_fbs =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());

  auto result = steamrot::subscriber_factory::CreateSubscriber(subscriber_fbs);

  REQUIRE(result.has_value());
  REQUIRE(result.value().m_trigger_event_type ==
          steamrot::EventType::EventType_EVENT_USER_INPUT);
  REQUIRE_FALSE(result.value().m_active);
  REQUIRE(result.value().m_trigger_event_data.has_value());
  REQUIRE(std::holds_alternative<steamrot::UserInputBitset>(
      result.value().m_trigger_event_data.value()));

  // Verify the bitset has A key pressed
  const auto &bitset = std::get<steamrot::UserInputBitset>(
      result.value().m_trigger_event_data.value());
  REQUIRE(bitset.test(static_cast<size_t>(sf::Keyboard::Key::A)));
}

TEST_CASE("CreateSubscriber creates subscriber with SceneChangePacket trigger "
          "data",
          "[unit][subscriber_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SceneChangePacketData
  auto trigger_data_offset =
      steamrot::CreateSceneChangePacketData(builder, 0, // uuid
                                            steamrot::SceneType_TITLE);

  // Create SubscriberFbs with trigger data
  auto subscriber_offset = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
      steamrot::EventDataData::EventDataData_SceneChangePacketData,
      trigger_data_offset.Union(), false);
  builder.Finish(subscriber_offset);

  const steamrot::SubscriberFbs *subscriber_fbs =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());

  auto result = steamrot::subscriber_factory::CreateSubscriber(subscriber_fbs);

  REQUIRE(result.has_value());
  REQUIRE(result.value().m_trigger_event_type ==
          steamrot::EventType::EventType_EVENT_CHANGE_SCENE);
  REQUIRE_FALSE(result.value().m_active);
  REQUIRE(result.value().m_trigger_event_data.has_value());
  REQUIRE(std::holds_alternative<steamrot::SceneChangePacket>(
      result.value().m_trigger_event_data.value()));

  // Verify the scene type
  const auto &scene_packet = std::get<steamrot::SceneChangePacket>(
      result.value().m_trigger_event_data.value());
  REQUIRE(scene_packet.second == steamrot::SceneType_TITLE);
  REQUIRE_FALSE(scene_packet.first.has_value()); // No UUID
}

TEST_CASE("CreateSubscriber creates subscriber with UserInterfaceName trigger "
          "data",
          "[unit][subscriber_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create UserInterfaceNameData
  auto ui_name_str = builder.CreateString("test_ui_element");
  auto trigger_data_offset =
      steamrot::CreateUserInterfaceNameData(builder, ui_name_str);

  // Create SubscriberFbs with trigger data
  auto subscriber_offset = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EventType_EVENT_TOGGLE_UI,
      steamrot::EventDataData::EventDataData_UserInterfaceNameData,
      trigger_data_offset.Union(), false);
  builder.Finish(subscriber_offset);

  const steamrot::SubscriberFbs *subscriber_fbs =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());

  auto result = steamrot::subscriber_factory::CreateSubscriber(subscriber_fbs);

  REQUIRE(result.has_value());
  REQUIRE(result.value().m_trigger_event_type ==
          steamrot::EventType::EventType_EVENT_TOGGLE_UI);
  REQUIRE_FALSE(result.value().m_active);
  REQUIRE(result.value().m_trigger_event_data.has_value());
  REQUIRE(std::holds_alternative<steamrot::UserInterfaceName>(
      result.value().m_trigger_event_data.value()));

  // Verify the UI name
  const auto &ui_name = std::get<steamrot::UserInterfaceName>(
      result.value().m_trigger_event_data.value());
  REQUIRE(ui_name == "test_ui_element");
}

TEST_CASE("CreateSubscriber propagates error from CreateEventData with "
          "invalid UUID",
          "[unit][subscriber_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SceneChangePacketData with invalid UUID
  auto uuid_str = builder.CreateString("invalid-uuid");
  auto trigger_data_offset = steamrot::CreateSceneChangePacketData(
      builder, uuid_str, steamrot::SceneType_TITLE);

  // Create SubscriberFbs with trigger data
  auto subscriber_offset = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
      steamrot::EventDataData::EventDataData_SceneChangePacketData,
      trigger_data_offset.Union(), false);
  builder.Finish(subscriber_offset);

  const steamrot::SubscriberFbs *subscriber_fbs =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());

  auto result = steamrot::subscriber_factory::CreateSubscriber(subscriber_fbs);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidUUID);
}

TEST_CASE("CreateSubscriber propagates error from CreateEventData with "
          "missing UI name",
          "[unit][subscriber_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create UserInterfaceNameData without id
  auto trigger_data_offset = steamrot::CreateUserInterfaceNameData(builder);

  // Create SubscriberFbs with trigger data
  auto subscriber_offset = steamrot::CreateSubscriberFbs(
      builder, steamrot::EventType::EventType_EVENT_TOGGLE_UI,
      steamrot::EventDataData::EventDataData_UserInterfaceNameData,
      trigger_data_offset.Union(), false);
  builder.Finish(subscriber_offset);

  const steamrot::SubscriberFbs *subscriber_fbs =
      flatbuffers::GetRoot<steamrot::SubscriberFbs>(builder.GetBufferPointer());

  auto result = steamrot::subscriber_factory::CreateSubscriber(subscriber_fbs);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}
