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
ConfigureUIPayload(UIPayload &ui_payload, const UIPayloadFbs *ui_payload_data) {
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
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
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
      return std::unexpected(FailInfo{FailMode::InvalidUUID,
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

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEventCategory(EventCategory &event_category,
                       EventCategoryFbs event_category_data) {
  // Convert flatbuffers enum to native enum
  switch (event_category_data) {
  case EventCategoryFbs_USER_INPUT:
    event_category = EventCategory::USER_INPUT;
    break;
  case EventCategoryFbs_UI:
    event_category = EventCategory::UI;
    break;
  case EventCategoryFbs_LOGIC:
    event_category = EventCategory::LOGIC;
    break;
  case EventCategoryFbs_SCENE:
    event_category = EventCategory::SCENE;
    break;
  case EventCategoryFbs_SYSTEM:
    event_category = EventCategory::SYSTEM;
    break;
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unknown EventCategoryFbs value in flatbuffers data"});
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEventType(EventType &event_type, EventTypeFbs event_type_data) {
  // Convert flatbuffers enum to native enum
  switch (event_type_data) {
  case EventTypeFbs_NONE:
    event_type = EventType::NONE;
    break;
  // User Input (100-199)
  case EventTypeFbs_USER_INPUT_KEYBOARD:
    event_type = EventType::USER_INPUT_KEYBOARD;
    break;
  case EventTypeFbs_USER_INPUT_MOUSE:
    event_type = EventType::USER_INPUT_MOUSE;
    break;
  case EventTypeFbs_USER_INPUT_GAMEPAD:
    event_type = EventType::USER_INPUT_GAMEPAD;
    break;
  // UI (200-299)
  case EventTypeFbs_UI_TOGGLE:
    event_type = EventType::UI_TOGGLE;
    break;
  // Scene (300-399)
  case EventTypeFbs_SCENE_CHANGE:
    event_type = EventType::SCENE_CHANGE;
    break;
  // Logic (400-499)
  case EventTypeFbs_LOGIC_TOGGLE:
    event_type = EventType::LOGIC_TOGGLE;
    break;
  // System (500-599)
  case EventTypeFbs_SYSTEM_QUIT:
    event_type = EventType::SYSTEM_QUIT;
    break;
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unknown EventTypeFbs value in flatbuffers data"});
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEventPayload(EventPayload &event_payload,
                      EventPayloadFbs event_payload_data,
                      const void *event_payload_ptr) {
  // Check for null payload pointer
  if (!event_payload_ptr) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "EventPayloadFbs data pointer is null, cannot populate "
                 "EventPayload"});
  }

  // Convert union type to appropriate payload variant
  switch (event_payload_data) {
  case EventPayloadFbs_InputPayloadFbs: {
    auto *input_payload_data =
        static_cast<const InputPayloadFbs *>(event_payload_ptr);
    InputPayload input_payload;
    auto result = ConfigureInputPayload(input_payload, input_payload_data);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
    event_payload = input_payload;
    break;
  }
  case EventPayloadFbs_UIPayloadFbs: {
    auto *ui_payload_data =
        static_cast<const UIPayloadFbs *>(event_payload_ptr);
    UIPayload ui_payload;
    auto result = ConfigureUIPayload(ui_payload, ui_payload_data);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
    event_payload = ui_payload;
    break;
  }
  case EventPayloadFbs_LogicPayloadFbs: {
    auto *logic_payload_data =
        static_cast<const LogicPayloadFbs *>(event_payload_ptr);
    LogicPayload logic_payload;
    auto result = ConfigureLogicPayload(logic_payload, logic_payload_data);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
    event_payload = logic_payload;
    break;
  }
  case EventPayloadFbs_ScenePayloadFbs: {
    auto *scene_payload_data =
        static_cast<const ScenePayloadFbs *>(event_payload_ptr);
    ScenePayload scene_payload;
    auto result = ConfigureScenePayload(scene_payload, scene_payload_data);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
    event_payload = scene_payload;
    break;
  }
  case EventPayloadFbs_SystemPayloadFbs: {
    auto *system_payload_data =
        static_cast<const SystemPayloadFbs *>(event_payload_ptr);
    SystemPayload system_payload;
    auto result = ConfigureSystemPayload(system_payload, system_payload_data);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
    event_payload = system_payload;
    break;
  }
  case EventPayloadFbs_NONE:
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "EventPayloadFbs union type is NONE, cannot populate "
                 "EventPayload"});
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unknown EventPayloadFbs union type in flatbuffers data"});
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEventPacket(EventPacket &event_packet,
                     const EventPacketFbs *event_packet_data) {
  // Check for null data
  if (!event_packet_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "EventPacketFbs data is null, cannot populate EventPacket"});
  }

  // Configure context (required field)
  if (!event_packet_data->context()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "EventPacketFbs missing required context field"});
  }
  auto context_result =
      ConfigureEventContext(event_packet.context, event_packet_data->context());
  if (!context_result.has_value()) {
    return std::unexpected(context_result.error());
  }

  // Configure category
  auto category_result = ConfigureEventCategory(event_packet.category,
                                                 event_packet_data->category());
  if (!category_result.has_value()) {
    return std::unexpected(category_result.error());
  }

  // Configure type
  auto type_result =
      ConfigureEventType(event_packet.type, event_packet_data->type());
  if (!type_result.has_value()) {
    return std::unexpected(type_result.error());
  }

  // Configure payload (required union field)
  if (event_packet_data->payload_type() == EventPayloadFbs_NONE) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "EventPacketFbs missing required payload field"});
  }
  auto payload_result =
      ConfigureEventPayload(event_packet.payload,
                            event_packet_data->payload_type(),
                            event_packet_data->payload());
  if (!payload_result.has_value()) {
    return std::unexpected(payload_result.error());
  }

  return std::monostate{};
}

} // namespace steamrot::data::configure
