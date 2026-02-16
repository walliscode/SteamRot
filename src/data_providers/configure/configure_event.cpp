/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions to configure event data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_event.h"
#include "SceneType.h"
#include "uuid.h"

namespace steamrot::data::configure {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEventContext(EventContext &event_context,
                      const EventContextFbs *event_context_data) {
  // check for null data
  if (!event_context_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "EventContextFbs data is null, cannot populate EventContext"});
  }

  // populate fields from flatbuffers data
  event_context.lifetime = event_context_data->lifetime();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureInputPayload(InputPayload &input_payload,
                      const InputPayloadFbs *input_payload_data) {
  // check for null data
  if (!input_payload_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "InputPayloadFbs data is null, cannot populate InputPayload"});
  }

  // populate fields from flatbuffers data
  switch (input_payload_data->action()) {
  case InputActionFbs_SELECT:
    input_payload.action = InputPayload::InputAction::SELECT;
    break;
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unknown InputActionFbs value in flatbuffers data"});
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureUIPayload(UIPayload &ui_payload,
                   const UIPayloadFbs *ui_payload_data) {
  // check for null data
  if (!ui_payload_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "UIPayloadFbs data is null, cannot populate UIPayload"});
  }

  // populate optional string fields
  if (ui_payload_data->c_user_interface_name()) {
    ui_payload.c_user_interface_name =
        ui_payload_data->c_user_interface_name()->str();
  }

  if (ui_payload_data->c_ui_state_name()) {
    ui_payload.c_ui_state_name = ui_payload_data->c_ui_state_name()->str();
  }

  // populate action enum
  switch (ui_payload_data->action()) {
  case UIActionFbs_TOGGLE:
    ui_payload.action = UIPayload::UIAction::TOGGLE;
    break;
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unknown UIActionFbs value in flatbuffers data"});
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureLogicPayload(LogicPayload &logic_payload,
                      const LogicPayloadFbs *logic_payload_data) {
  // check for null data
  if (!logic_payload_data) {
    return std::unexpected(FailInfo{
        FailMode::FlatbuffersDataNotFound,
        "LogicPayloadFbs data is null, cannot populate LogicPayload"});
  }

  // populate toggle_name enum
  switch (logic_payload_data->toggle_name()) {
  case LogicToggleFbs_INITIATE_MACHINA_FORM_SCAFFOLD:
    logic_payload.toggle_name =
        LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD;
    break;
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unknown LogicToggleFbs value in flatbuffers data"});
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureScenePayload(ScenePayload &scene_payload,
                      const ScenePayloadFbs *scene_payload_data) {
  // check for null data
  if (!scene_payload_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "ScenePayloadFbs data is null, cannot populate ScenePayload"});
  }

  // populate optional scene_type
  if (scene_payload_data->scene_type() != SceneTypeFbs_UNKNOWN) {
    switch (scene_payload_data->scene_type()) {
    case SceneTypeFbs_TEST:
      scene_payload.scene_type = SceneType::TEST;
      break;
    case SceneTypeFbs_TITLE:
      scene_payload.scene_type = SceneType::TITLE;
      break;
    case SceneTypeFbs_CRAFTING:
      scene_payload.scene_type = SceneType::CRAFTING;
      break;
    case SceneTypeFbs_UNKNOWN:
      // Already handled above, but included for completeness
      break;
    default:
      return std::unexpected(
          FailInfo{FailMode::NonExistentEnumValue,
                   "Unknown SceneTypeFbs value in flatbuffers data"});
    }
  }

  // populate optional scene_id
  if (scene_payload_data->scene_id()) {
    std::string uuid_str = scene_payload_data->scene_id()->str();
    auto uuid_result = uuids::uuid::from_string(uuid_str);
    if (!uuid_result.has_value()) {
      return std::unexpected(FailInfo{FailMode::InvalidData,
                                       "Invalid UUID string in scene_id field"});
    }
    scene_payload.scene_id = uuid_result.value();
  }

  // populate action enum
  switch (scene_payload_data->action()) {
  case SceneActionFbs_CHANGE:
    scene_payload.action = ScenePayload::SceneAction::CHANGE;
    break;
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unknown SceneActionFbs value in flatbuffers data"});
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSystemPayload(SystemPayload &system_payload,
                       const SystemPayloadFbs *system_payload_data) {
  // check for null data
  if (!system_payload_data) {
    return std::unexpected(FailInfo{
        FailMode::FlatbuffersDataNotFound,
        "SystemPayloadFbs data is null, cannot populate SystemPayload"});
  }

  // populate action enum
  switch (system_payload_data->action()) {
  case SystemActionFbs_QUIT:
    system_payload.action = SystemPayload::SystemAction::QUIT;
    break;
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unknown SystemActionFbs value in flatbuffers data"});
  }

  return std::monostate{};
}

} // namespace steamrot::data::configure
