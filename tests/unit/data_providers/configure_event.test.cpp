/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for configure_event functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_event.h"
#include "EventContext.h"
#include "EventPayload.h"
#include "event_context_generated.h"
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
