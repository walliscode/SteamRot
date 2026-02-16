/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for configure_event functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_event.h"
#include "EventContext.h"
#include "EventPacket.h"
#include "EventPayload.h"
#include "EventType.h"
#include "event_context_generated.h"
#include "event_packet_generated.h"
#include "event_payload_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>

/////////////////////////////////////////////////
// ConfigureEventContext tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureEventContext fails with null data",
          "[unit][configure_event]") {
  steamrot::EventContext context;

  auto result =
      steamrot::data::configure::ConfigureEventContext(context, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureEventContext populates from valid data",
          "[unit][configure_event]") {
  // Create test flatbuffers data
  flatbuffers::FlatBufferBuilder builder;
  auto context_data =
      steamrot::CreateEventContextFbs(builder, 5); // lifetime = 5
  builder.Finish(context_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::EventContextFbs>(
      builder.GetBufferPointer());

  steamrot::EventContext context;
  auto result =
      steamrot::data::configure::ConfigureEventContext(context, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(context.lifetime == 5);
}

/////////////////////////////////////////////////
// ConfigureInputPayload tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureInputPayload fails with null data",
          "[unit][configure_event]") {
  steamrot::InputPayload payload;

  auto result =
      steamrot::data::configure::ConfigureInputPayload(payload, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureInputPayload populates from valid data",
          "[unit][configure_event]") {
  // Create test flatbuffers data
  flatbuffers::FlatBufferBuilder builder;
  auto payload_data =
      steamrot::CreateInputPayloadFbs(builder, steamrot::InputActionFbs_SELECT);
  builder.Finish(payload_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::InputPayloadFbs>(
      builder.GetBufferPointer());

  steamrot::InputPayload payload;
  auto result =
      steamrot::data::configure::ConfigureInputPayload(payload, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(payload.action == steamrot::InputPayload::InputAction::SELECT);
}

/////////////////////////////////////////////////
// ConfigureUIPayload tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureUIPayload fails with null data",
          "[unit][configure_event]") {
  steamrot::UIPayload payload;

  auto result = steamrot::data::configure::ConfigureUIPayload(payload, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureUIPayload populates from valid data",
          "[unit][configure_event]") {
  // Create test flatbuffers data
  flatbuffers::FlatBufferBuilder builder;
  auto ui_name = builder.CreateString("test_ui");
  auto state_name = builder.CreateString("test_state");
  auto payload_data = steamrot::CreateUIPayloadFbs(
      builder, ui_name, state_name, steamrot::UIActionFbs_TOGGLE);
  builder.Finish(payload_data);

  auto *fbs_data =
      flatbuffers::GetRoot<steamrot::UIPayloadFbs>(builder.GetBufferPointer());

  steamrot::UIPayload payload;
  auto result =
      steamrot::data::configure::ConfigureUIPayload(payload, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(payload.c_user_interface_name.has_value());
  REQUIRE(payload.c_user_interface_name.value() == "test_ui");
  REQUIRE(payload.c_ui_state_name.has_value());
  REQUIRE(payload.c_ui_state_name.value() == "test_state");
  REQUIRE(payload.action == steamrot::UIPayload::UIAction::TOGGLE);
}

TEST_CASE("ConfigureUIPayload handles optional fields",
          "[unit][configure_event]") {
  // Create test flatbuffers data with no optional fields
  flatbuffers::FlatBufferBuilder builder;
  auto payload_data =
      steamrot::CreateUIPayloadFbs(builder, 0, 0, steamrot::UIActionFbs_TOGGLE);
  builder.Finish(payload_data);

  auto *fbs_data =
      flatbuffers::GetRoot<steamrot::UIPayloadFbs>(builder.GetBufferPointer());

  steamrot::UIPayload payload;
  auto result =
      steamrot::data::configure::ConfigureUIPayload(payload, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE_FALSE(payload.c_user_interface_name.has_value());
  REQUIRE_FALSE(payload.c_ui_state_name.has_value());
  REQUIRE(payload.action == steamrot::UIPayload::UIAction::TOGGLE);
}

/////////////////////////////////////////////////
// ConfigureLogicPayload tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureLogicPayload fails with null data",
          "[unit][configure_event]") {
  steamrot::LogicPayload payload;

  auto result =
      steamrot::data::configure::ConfigureLogicPayload(payload, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureLogicPayload populates from valid data",
          "[unit][configure_event]") {
  // Create test flatbuffers data
  flatbuffers::FlatBufferBuilder builder;
  auto payload_data = steamrot::CreateLogicPayloadFbs(
      builder, steamrot::LogicToggleFbs_INITIATE_MACHINA_FORM_SCAFFOLD);
  builder.Finish(payload_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::LogicPayloadFbs>(
      builder.GetBufferPointer());

  steamrot::LogicPayload payload;
  auto result =
      steamrot::data::configure::ConfigureLogicPayload(payload, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(payload.toggle_name ==
          steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD);
}

/////////////////////////////////////////////////
// ConfigureScenePayload tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureScenePayload fails with null data",
          "[unit][configure_event]") {
  steamrot::ScenePayload payload;

  auto result =
      steamrot::data::configure::ConfigureScenePayload(payload, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureScenePayload populates from valid data",
          "[unit][configure_event]") {
  // Create test flatbuffers data
  flatbuffers::FlatBufferBuilder builder;
  auto uuid_str = builder.CreateString("550e8400-e29b-41d4-a716-446655440000");
  auto payload_data = steamrot::CreateScenePayloadFbs(
      builder, steamrot::SceneTypeFbs_TITLE, uuid_str,
      steamrot::SceneActionFbs_CHANGE);
  builder.Finish(payload_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::ScenePayloadFbs>(
      builder.GetBufferPointer());

  steamrot::ScenePayload payload;
  auto result =
      steamrot::data::configure::ConfigureScenePayload(payload, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(payload.scene_type.has_value());
  REQUIRE(payload.scene_type.value() == steamrot::SceneType::TITLE);
  REQUIRE(payload.scene_id.has_value());
  REQUIRE(payload.action == steamrot::ScenePayload::SceneAction::CHANGE);
}

TEST_CASE("ConfigureScenePayload handles optional fields",
          "[unit][configure_event]") {
  // Create test flatbuffers data with no optional fields
  flatbuffers::FlatBufferBuilder builder;
  auto payload_data =
      steamrot::CreateScenePayloadFbs(builder, steamrot::SceneTypeFbs_UNKNOWN,
                                      0, steamrot::SceneActionFbs_CHANGE);
  builder.Finish(payload_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::ScenePayloadFbs>(
      builder.GetBufferPointer());

  steamrot::ScenePayload payload;
  auto result =
      steamrot::data::configure::ConfigureScenePayload(payload, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE_FALSE(payload.scene_type.has_value());
  REQUIRE_FALSE(payload.scene_id.has_value());
  REQUIRE(payload.action == steamrot::ScenePayload::SceneAction::CHANGE);
}

TEST_CASE("ConfigureScenePayload handles invalid UUID",
          "[unit][configure_event]") {
  // Create test flatbuffers data with invalid UUID
  flatbuffers::FlatBufferBuilder builder;
  auto uuid_str = builder.CreateString("invalid-uuid-format");
  auto payload_data = steamrot::CreateScenePayloadFbs(
      builder, steamrot::SceneTypeFbs_TITLE, uuid_str,
      steamrot::SceneActionFbs_CHANGE);
  builder.Finish(payload_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::ScenePayloadFbs>(
      builder.GetBufferPointer());

  steamrot::ScenePayload payload;
  auto result =
      steamrot::data::configure::ConfigureScenePayload(payload, fbs_data);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidUUID);
}

/////////////////////////////////////////////////
// ConfigureSystemPayload tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureSystemPayload fails with null data",
          "[unit][configure_event]") {
  steamrot::SystemPayload payload;

  auto result =
      steamrot::data::configure::ConfigureSystemPayload(payload, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureSystemPayload populates from valid data",
          "[unit][configure_event]") {
  // Create test flatbuffers data
  flatbuffers::FlatBufferBuilder builder;
  auto payload_data =
      steamrot::CreateSystemPayloadFbs(builder, steamrot::SystemActionFbs_QUIT);
  builder.Finish(payload_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::SystemPayloadFbs>(
      builder.GetBufferPointer());

  steamrot::SystemPayload payload;
  auto result =
      steamrot::data::configure::ConfigureSystemPayload(payload, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(payload.action == steamrot::SystemPayload::SystemAction::QUIT);
}

/////////////////////////////////////////////////
// ConfigureEventType tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureEventType populates NONE type", "[unit][configure_event]") {
  steamrot::EventType type;

  auto result = steamrot::data::configure::ConfigureEventType(
      type, steamrot::EventTypeFbs_NONE);

  REQUIRE(result.has_value());
  REQUIRE(type == steamrot::EventType::NONE);
}

TEST_CASE("ConfigureEventType populates USER_INPUT_KEYBOARD type",
          "[unit][configure_event]") {
  steamrot::EventType type;

  auto result = steamrot::data::configure::ConfigureEventType(
      type, steamrot::EventTypeFbs_USER_INPUT_KEYBOARD);

  REQUIRE(result.has_value());
  REQUIRE(type == steamrot::EventType::USER_INPUT_KEYBOARD);
}

TEST_CASE("ConfigureEventType populates USER_INPUT_MOUSE type",
          "[unit][configure_event]") {
  steamrot::EventType type;

  auto result = steamrot::data::configure::ConfigureEventType(
      type, steamrot::EventTypeFbs_USER_INPUT_MOUSE);

  REQUIRE(result.has_value());
  REQUIRE(type == steamrot::EventType::USER_INPUT_MOUSE);
}

TEST_CASE("ConfigureEventType populates USER_INPUT_GAMEPAD type",
          "[unit][configure_event]") {
  steamrot::EventType type;

  auto result = steamrot::data::configure::ConfigureEventType(
      type, steamrot::EventTypeFbs_USER_INPUT_GAMEPAD);

  REQUIRE(result.has_value());
  REQUIRE(type == steamrot::EventType::USER_INPUT_GAMEPAD);
}

TEST_CASE("ConfigureEventType populates UI_TOGGLE type",
          "[unit][configure_event]") {
  steamrot::EventType type;

  auto result = steamrot::data::configure::ConfigureEventType(
      type, steamrot::EventTypeFbs_UI_TOGGLE);

  REQUIRE(result.has_value());
  REQUIRE(type == steamrot::EventType::UI_TOGGLE);
}

TEST_CASE("ConfigureEventType populates SCENE_CHANGE type",
          "[unit][configure_event]") {
  steamrot::EventType type;

  auto result = steamrot::data::configure::ConfigureEventType(
      type, steamrot::EventTypeFbs_SCENE_CHANGE);

  REQUIRE(result.has_value());
  REQUIRE(type == steamrot::EventType::SCENE_CHANGE);
}

TEST_CASE("ConfigureEventType populates LOGIC_TOGGLE type",
          "[unit][configure_event]") {
  steamrot::EventType type;

  auto result = steamrot::data::configure::ConfigureEventType(
      type, steamrot::EventTypeFbs_LOGIC_TOGGLE);

  REQUIRE(result.has_value());
  REQUIRE(type == steamrot::EventType::LOGIC_TOGGLE);
}

TEST_CASE("ConfigureEventType populates SYSTEM_QUIT type",
          "[unit][configure_event]") {
  steamrot::EventType type;

  auto result = steamrot::data::configure::ConfigureEventType(
      type, steamrot::EventTypeFbs_SYSTEM_QUIT);

  REQUIRE(result.has_value());
  REQUIRE(type == steamrot::EventType::SYSTEM_QUIT);
}

/////////////////////////////////////////////////
// ConfigureEventPayload tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureEventPayload fails with null pointer",
          "[unit][configure_event]") {
  steamrot::EventPayload payload;

  auto result = steamrot::data::configure::ConfigureEventPayload(
      payload, steamrot::EventPayloadFbs_InputPayloadFbs, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureEventPayload fails with NONE union type",
          "[unit][configure_event]") {
  steamrot::EventPayload payload;
  int dummy_ptr = 0;

  auto result = steamrot::data::configure::ConfigureEventPayload(
      payload, steamrot::EventPayloadFbs_NONE, &dummy_ptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureEventPayload populates InputPayload",
          "[unit][configure_event]") {
  flatbuffers::FlatBufferBuilder builder;
  auto input_payload_data =
      steamrot::CreateInputPayloadFbs(builder, steamrot::InputActionFbs_SELECT);
  builder.Finish(input_payload_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::InputPayloadFbs>(
      builder.GetBufferPointer());

  steamrot::EventPayload payload;
  auto result = steamrot::data::configure::ConfigureEventPayload(
      payload, steamrot::EventPayloadFbs_InputPayloadFbs, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(std::holds_alternative<steamrot::InputPayload>(payload));
  auto &input = std::get<steamrot::InputPayload>(payload);
  REQUIRE(input.action == steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("ConfigureEventPayload populates UIPayload",
          "[unit][configure_event]") {
  flatbuffers::FlatBufferBuilder builder;
  auto ui_name = builder.CreateString("test_ui");
  auto state_name = builder.CreateString("test_state");
  auto ui_payload_data = steamrot::CreateUIPayloadFbs(
      builder, ui_name, state_name, steamrot::UIActionFbs_TOGGLE);
  builder.Finish(ui_payload_data);

  auto *fbs_data =
      flatbuffers::GetRoot<steamrot::UIPayloadFbs>(builder.GetBufferPointer());

  steamrot::EventPayload payload;
  auto result = steamrot::data::configure::ConfigureEventPayload(
      payload, steamrot::EventPayloadFbs_UIPayloadFbs, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(std::holds_alternative<steamrot::UIPayload>(payload));
  auto &ui = std::get<steamrot::UIPayload>(payload);
  REQUIRE(ui.c_user_interface_name.has_value());
  REQUIRE(ui.c_user_interface_name.value() == "test_ui");
  REQUIRE(ui.action == steamrot::UIPayload::UIAction::TOGGLE);
}

/////////////////////////////////////////////////
// ConfigureEventPacket tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureEventPacket fails with null data",
          "[unit][configure_event]") {
  steamrot::EventPacket packet;

  auto result =
      steamrot::data::configure::ConfigureEventPacket(packet, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureEventPacket fails with missing context",
          "[unit][configure_event]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create payload
  auto input_payload_data =
      steamrot::CreateInputPayloadFbs(builder, steamrot::InputActionFbs_SELECT);

  // Create packet without context (pass 0 for context)
  auto packet_data = steamrot::CreateEventPacketFbs(
      builder, 0, steamrot::EventCategoryFbs_USER_INPUT,
      steamrot::EventTypeFbs_USER_INPUT_KEYBOARD,
      steamrot::EventPayloadFbs_InputPayloadFbs, input_payload_data.Union());
  builder.Finish(packet_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::EventPacketFbs>(
      builder.GetBufferPointer());

  steamrot::EventPacket packet;
  auto result =
      steamrot::data::configure::ConfigureEventPacket(packet, fbs_data);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureEventPacket fails with missing payload",
          "[unit][configure_event]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create context
  auto context_data = steamrot::CreateEventContextFbs(builder, 3);

  // Create packet with NONE payload
  auto packet_data = steamrot::CreateEventPacketFbs(
      builder, context_data, steamrot::EventCategoryFbs_USER_INPUT,
      steamrot::EventTypeFbs_USER_INPUT_KEYBOARD);
  builder.Finish(packet_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::EventPacketFbs>(
      builder.GetBufferPointer());

  steamrot::EventPacket packet;
  auto result =
      steamrot::data::configure::ConfigureEventPacket(packet, fbs_data);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureEventPacket populates complete packet",
          "[unit][configure_event]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create context
  auto context_data = steamrot::CreateEventContextFbs(builder, 5);

  // Create payload
  auto input_payload_data =
      steamrot::CreateInputPayloadFbs(builder, steamrot::InputActionFbs_SELECT);

  // Create packet
  auto packet_data = steamrot::CreateEventPacketFbs(
      builder, context_data, steamrot::EventCategoryFbs_USER_INPUT,
      steamrot::EventTypeFbs_USER_INPUT_KEYBOARD,
      steamrot::EventPayloadFbs_InputPayloadFbs, input_payload_data.Union());
  builder.Finish(packet_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::EventPacketFbs>(
      builder.GetBufferPointer());

  steamrot::EventPacket packet;
  auto result =
      steamrot::data::configure::ConfigureEventPacket(packet, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(packet.context.lifetime == 5);
  REQUIRE(packet.category == steamrot::EventCategory::USER_INPUT);
  REQUIRE(packet.type == steamrot::EventType::USER_INPUT_KEYBOARD);
  REQUIRE(std::holds_alternative<steamrot::InputPayload>(packet.payload));
  auto &input = std::get<steamrot::InputPayload>(packet.payload);
  REQUIRE(input.action == steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("ConfigureEventPacket populates with UIPayload",
          "[unit][configure_event]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create context
  auto context_data = steamrot::CreateEventContextFbs(builder, 2);

  // Create payload
  auto ui_name = builder.CreateString("menu_ui");
  auto ui_payload_data = steamrot::CreateUIPayloadFbs(
      builder, ui_name, 0, steamrot::UIActionFbs_TOGGLE);

  // Create packet
  auto packet_data = steamrot::CreateEventPacketFbs(
      builder, context_data, steamrot::EventCategoryFbs_UI,
      steamrot::EventTypeFbs_UI_TOGGLE, steamrot::EventPayloadFbs_UIPayloadFbs,
      ui_payload_data.Union());
  builder.Finish(packet_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::EventPacketFbs>(
      builder.GetBufferPointer());

  steamrot::EventPacket packet;
  auto result =
      steamrot::data::configure::ConfigureEventPacket(packet, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(packet.context.lifetime == 2);
  REQUIRE(packet.category == steamrot::EventCategory::UI);
  REQUIRE(packet.type == steamrot::EventType::UI_TOGGLE);
  REQUIRE(std::holds_alternative<steamrot::UIPayload>(packet.payload));
  auto &ui = std::get<steamrot::UIPayload>(packet.payload);
  REQUIRE(ui.c_user_interface_name.has_value());
  REQUIRE(ui.c_user_interface_name.value() == "menu_ui");
  REQUIRE(ui.action == steamrot::UIPayload::UIAction::TOGGLE);
}

TEST_CASE("ConfigureEventPacket populates with SystemPayload",
          "[unit][configure_event]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create context
  auto context_data = steamrot::CreateEventContextFbs(builder, 1);

  // Create payload
  auto system_payload_data =
      steamrot::CreateSystemPayloadFbs(builder, steamrot::SystemActionFbs_QUIT);

  // Create packet
  auto packet_data = steamrot::CreateEventPacketFbs(
      builder, context_data, steamrot::EventCategoryFbs_SYSTEM,
      steamrot::EventTypeFbs_SYSTEM_QUIT,
      steamrot::EventPayloadFbs_SystemPayloadFbs, system_payload_data.Union());
  builder.Finish(packet_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::EventPacketFbs>(
      builder.GetBufferPointer());

  steamrot::EventPacket packet;
  auto result =
      steamrot::data::configure::ConfigureEventPacket(packet, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(packet.context.lifetime == 1);
  REQUIRE(packet.category == steamrot::EventCategory::SYSTEM);
  REQUIRE(packet.type == steamrot::EventType::SYSTEM_QUIT);
  REQUIRE(std::holds_alternative<steamrot::SystemPayload>(packet.payload));
  auto &system = std::get<steamrot::SystemPayload>(packet.payload);
  REQUIRE(system.action == steamrot::SystemPayload::SystemAction::QUIT);
}
