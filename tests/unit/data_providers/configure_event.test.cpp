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

  auto state_name = builder.CreateString("test_state");
  auto payload_data = steamrot::CreateUIPayloadFbs(
      builder, state_name, steamrot::UIActionFbs_TOGGLE);
  builder.Finish(payload_data);

  auto *fbs_data =
      flatbuffers::GetRoot<steamrot::UIPayloadFbs>(builder.GetBufferPointer());

  steamrot::UIPayload payload;
  auto result =
      steamrot::data::configure::ConfigureUIPayload(payload, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(payload.c_ui_state_name == "test_state");
  REQUIRE(payload.action == steamrot::UIPayload::UIAction::TOGGLE);
}

TEST_CASE("ConfigureUIPayload handles optional fields",
          "[unit][configure_event]") {
  // Create test flatbuffers data with no optional fields
  flatbuffers::FlatBufferBuilder builder;
  auto payload_data =
      steamrot::CreateUIPayloadFbs(builder, 0, steamrot::UIActionFbs_TOGGLE);
  builder.Finish(payload_data);

  auto *fbs_data =
      flatbuffers::GetRoot<steamrot::UIPayloadFbs>(builder.GetBufferPointer());

  steamrot::UIPayload payload;
  auto result =
      steamrot::data::configure::ConfigureUIPayload(payload, fbs_data);

  REQUIRE(result.has_value());

  REQUIRE(payload.c_ui_state_name.empty());
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
  REQUIRE(payload.scene_type == steamrot::SceneType::TITLE);
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
  REQUIRE_FALSE(payload.scene_type == steamrot::SceneType::UNKNOWN);
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

TEST_CASE("ConfigureEventType populates USER_INPUT type",
          "[unit][configure_event]") {
  steamrot::EventType type;

  auto result = steamrot::data::configure::ConfigureEventType(
      type, steamrot::EventTypeFbs_USER_INPUT);

  REQUIRE(result.has_value());
  REQUIRE(type == steamrot::EventType::USER_INPUT);
}

TEST_CASE("ConfigureEventType populates UI type", "[unit][configure_event]") {
  steamrot::EventType type;

  auto result = steamrot::data::configure::ConfigureEventType(
      type, steamrot::EventTypeFbs_UI);

  REQUIRE(result.has_value());
  REQUIRE(type == steamrot::EventType::UI);
}

TEST_CASE("ConfigureEventType populates LOGIC type",
          "[unit][configure_event]") {
  steamrot::EventType type;

  auto result = steamrot::data::configure::ConfigureEventType(
      type, steamrot::EventTypeFbs_LOGIC);

  REQUIRE(result.has_value());
  REQUIRE(type == steamrot::EventType::LOGIC);
}

TEST_CASE("ConfigureEventType populates SCENE type",
          "[unit][configure_event]") {
  steamrot::EventType type;

  auto result = steamrot::data::configure::ConfigureEventType(
      type, steamrot::EventTypeFbs_SCENE);

  REQUIRE(result.has_value());
  REQUIRE(type == steamrot::EventType::SCENE);
}

TEST_CASE("ConfigureEventType populates SYSTEM type",
          "[unit][configure_event]") {
  steamrot::EventType type;

  auto result = steamrot::data::configure::ConfigureEventType(
      type, steamrot::EventTypeFbs_SYSTEM);

  REQUIRE(result.has_value());
  REQUIRE(type == steamrot::EventType::SYSTEM);
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
  auto state_name = builder.CreateString("test_state");
  auto ui_payload_data = steamrot::CreateUIPayloadFbs(
      builder, state_name, steamrot::UIActionFbs_TOGGLE);
  builder.Finish(ui_payload_data);

  auto *fbs_data =
      flatbuffers::GetRoot<steamrot::UIPayloadFbs>(builder.GetBufferPointer());

  steamrot::EventPayload payload;
  auto result = steamrot::data::configure::ConfigureEventPayload(
      payload, steamrot::EventPayloadFbs_UIPayloadFbs, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(std::holds_alternative<steamrot::UIPayload>(payload));
  auto &ui = std::get<steamrot::UIPayload>(payload);
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
      builder, context_data, steamrot::EventTypeFbs_USER_INPUT,
      steamrot::EventPayloadFbs_InputPayloadFbs, input_payload_data.Union());
  builder.Finish(packet_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::EventPacketFbs>(
      builder.GetBufferPointer());

  steamrot::EventPacket packet;
  auto result =
      steamrot::data::configure::ConfigureEventPacket(packet, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(packet.context.lifetime == 5);
  REQUIRE(packet.type == steamrot::EventType::USER_INPUT);
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
  auto ui_payload_data =
      steamrot::CreateUIPayloadFbs(builder, 0, steamrot::UIActionFbs_TOGGLE);

  // Create packet
  auto packet_data = steamrot::CreateEventPacketFbs(
      builder, context_data, steamrot::EventTypeFbs_UI,
      steamrot::EventPayloadFbs_UIPayloadFbs, ui_payload_data.Union());
  builder.Finish(packet_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::EventPacketFbs>(
      builder.GetBufferPointer());

  steamrot::EventPacket packet;
  auto result =
      steamrot::data::configure::ConfigureEventPacket(packet, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(packet.context.lifetime == 2);
  REQUIRE(packet.type == steamrot::EventType::UI);
  REQUIRE(std::holds_alternative<steamrot::UIPayload>(packet.payload));
  auto &ui = std::get<steamrot::UIPayload>(packet.payload);
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
      builder, context_data, steamrot::EventTypeFbs_SYSTEM,
      steamrot::EventPayloadFbs_SystemPayloadFbs, system_payload_data.Union());
  builder.Finish(packet_data);

  auto *fbs_data = flatbuffers::GetRoot<steamrot::EventPacketFbs>(
      builder.GetBufferPointer());

  steamrot::EventPacket packet;
  auto result =
      steamrot::data::configure::ConfigureEventPacket(packet, fbs_data);

  REQUIRE(result.has_value());
  REQUIRE(packet.context.lifetime == 1);
  REQUIRE(packet.type == steamrot::EventType::SYSTEM);
  REQUIRE(std::holds_alternative<steamrot::SystemPayload>(packet.payload));
  auto &system = std::get<steamrot::SystemPayload>(packet.payload);
  REQUIRE(system.action == steamrot::SystemPayload::SystemAction::QUIT);
}
