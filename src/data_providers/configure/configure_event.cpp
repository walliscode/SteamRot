/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions to configure event data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_event.h"
#include "SceneType.h"
#include "configure_input_action.h"
#include "event_packet_generated.h"
#include "event_payload_generated.h"
#include "uuid.h"
#include <format>

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

  return ConfigureInputAction(input_payload.action,
                              input_payload_data->action());
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

  // populate string fields
  if (ui_payload_data->c_ui_state_name()) {
    ui_payload.c_ui_state_name = ui_payload_data->c_ui_state_name()->str();
  }

  // populate action enum
  switch (ui_payload_data->action()) {
  case UIActionFbs_TOGGLE:
    ui_payload.action = UIPayload::UIAction::TOGGLE;
    break;

  default:
    std::string action_name = EnumNameUIActionFbs(ui_payload_data->action());
    return std::unexpected(FailInfo{
        FailMode::NonExistentEnumValue,
        std::format("UIActionFBS enum is not accounted for {}", action_name)});
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
  case LogicToggleFbs_CLEAR_MACHINA_FORM_SCAFFOLD:
    logic_payload.toggle_name =
        LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD;
    break;
  case LogicToggleFbs_PERFORM_STRUCTURAL_ANALYSIS:
    logic_payload.toggle_name =
        LogicPayload::LogicToggle::PERFORM_STRUCTURAL_ANALYSIS;
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
      scene_payload.optional_scene_type = SceneType::TEST;
      break;
    case SceneTypeFbs_TITLE:
      scene_payload.optional_scene_type = SceneType::TITLE;
      break;
    case SceneTypeFbs_CRAFTING:
      scene_payload.optional_scene_type = SceneType::CRAFTING;
      break;
    case SceneTypeFbs_UNKNOWN:
      return std::unexpected(
          FailInfo{FailMode::MissingData,
                   "ScenePayloadFbs scene_type is UNKNOWN, cannot populate "
                   "ScenePayload.scene_type"});

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
    scene_payload.optional_scene_id = uuid_result.value();
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
ConfigureGhostPayload(GhostPayload &ghost_payload,
                      const GhostPayloadFbs *ghost_payload_data) {
  // check for null data
  if (!ghost_payload_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "GhostPayloadFbs data is null, cannot populate GhostPayload"});
  }

  // populate action enum
  switch (ghost_payload_data->action()) {
  case GhostActionFbs_NONE:
    ghost_payload.action = GhostPayload::GhostAction::NONE;
    break;
  case GhostActionFbs_SELECT:
    ghost_payload.action = GhostPayload::GhostAction::SELECT;
    break;
  case GhostActionFbs_CLEAR:
    ghost_payload.action = GhostPayload::GhostAction::CLEAR;
    break;
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unknown GhostActionFbs value in flatbuffers data"});
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureCameraPayload(CameraPayload &camera_payload,
                       const CameraPayloadFbs *camera_payload_data) {
  // check for null data
  if (!camera_payload_data) {
    return std::unexpected(FailInfo{
        FailMode::FlatbuffersDataNotFound,
        "CameraPayloadFbs data is null, cannot populate CameraPayload"});
  }

  switch (camera_payload_data->action()) {
  case CameraActionFbs_NONE:
    camera_payload.action = CameraPayload::CameraAction::NONE;
    break;
  case CameraActionFbs_SCROLL:
    camera_payload.action = CameraPayload::CameraAction::SCROLL;
    break;
  case CameraActionFbs_RESET_ZOOM:
    camera_payload.action = CameraPayload::CameraAction::RESET_ZOOM;
    break;
  case CameraActionFbs_PAN_PRESS:
    camera_payload.action = CameraPayload::CameraAction::PAN_PRESS;
    break;
  case CameraActionFbs_PAN_RELEASE:
    camera_payload.action = CameraPayload::CameraAction::PAN_RELEASE;
    break;
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unknown CameraActionFbs value in flatbuffers data"});
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
  case EventTypeFbs_USER_INPUT:
    event_type = EventType::USER_INPUT;
    break;
  case EventTypeFbs_UI:
    event_type = EventType::UI;
    break;
  case EventTypeFbs_LOGIC:
    event_type = EventType::LOGIC;
    break;
  case EventTypeFbs_SCENE:
    event_type = EventType::SCENE;
    break;
  case EventTypeFbs_SYSTEM:
    event_type = EventType::SYSTEM;
    break;
  case EventTypeFbs_GHOST:
    event_type = EventType::GHOST;
    break;
  case EventTypeFbs_CAMERA:
    event_type = EventType::CAMERA;
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
    if (!input_payload_data) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "InputPayloadFbs data pointer is null, cannot populate "
                   "InputPayload"});
    }
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
    if (!ui_payload_data) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "UIPayloadFbs data pointer is null, cannot populate "
                   "UIPayload"});
    }
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
    if (!logic_payload_data) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "LogicPayloadFbs data pointer is null, cannot populate "
                   "LogicPayload"});
    }
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

    if (!scene_payload_data) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "ScenePayloadFbs data pointer is null, cannot populate "
                   "ScenePayload"});
    }
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

    if (!system_payload_data) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "SystemPayloadFbs data pointer is null, cannot populate "
                   "SystemPayload"});
    }
    SystemPayload system_payload;
    auto result = ConfigureSystemPayload(system_payload, system_payload_data);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
    event_payload = system_payload;
    break;
  }
  case EventPayloadFbs_GhostPayloadFbs: {
    auto *ghost_payload_data =
        static_cast<const GhostPayloadFbs *>(event_payload_ptr);

    if (!ghost_payload_data) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "GhostPayloadFbs data pointer is null, cannot populate "
                   "GhostPayload"});
    }
    GhostPayload ghost_payload;
    auto result = ConfigureGhostPayload(ghost_payload, ghost_payload_data);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
    event_payload = ghost_payload;
    break;
  }
  case EventPayloadFbs_CameraPayloadFbs: {
    auto *camera_payload_data =
        static_cast<const CameraPayloadFbs *>(event_payload_ptr);

    if (!camera_payload_data) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "CameraPayloadFbs data pointer is null, cannot populate "
                   "CameraPayload"});
    }
    CameraPayload camera_payload;
    auto result = ConfigureCameraPayload(camera_payload, camera_payload_data);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
    event_payload = camera_payload;
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
  auto payload_result = ConfigureEventPayload(event_packet.payload,
                                              event_packet_data->payload_type(),
                                              event_packet_data->payload());
  if (!payload_result.has_value()) {
    return std::unexpected(payload_result.error());
  }

  return std::monostate{};
}

} // namespace steamrot::data::configure
