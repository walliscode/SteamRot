/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions related to event configuration
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventContext.h"
#include "EventPacket.h"
#include "EventPayload.h"
#include "EventType.h"
#include "FailInfo.h"
#include "event_context_generated.h"
#include "event_packet_generated.h"
#include "event_payload_generated.h"
#include <expected>

namespace steamrot::data::configure {

/////////////////////////////////////////////////
/// @brief Logic for populating EventContext from flatbuffers data source.
///
/// @param event_context Reference to EventContext to populate.
/// @param event_context_data Pointer to EventContextFbs flatbuffers data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEventContext(EventContext &event_context,
                      const EventContextFbs *event_context_data);

/////////////////////////////////////////////////
/// @brief Logic for populating InputPayload from flatbuffers data source.
///
/// @param input_payload Reference to InputPayload to populate.
/// @param input_payload_data Pointer to InputPayloadFbs flatbuffers data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureInputPayload(InputPayload &input_payload,
                      const InputPayloadFbs *input_payload_data);

/////////////////////////////////////////////////
/// @brief Logic for populating UIPayload from flatbuffers data source.
///
/// @param ui_payload Reference to UIPayload to populate.
/// @param ui_payload_data Pointer to UIPayloadFbs flatbuffers data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureUIPayload(UIPayload &ui_payload, const UIPayloadFbs *ui_payload_data);

/////////////////////////////////////////////////
/// @brief Logic for populating LogicPayload from flatbuffers data source.
///
/// @param logic_payload Reference to LogicPayload to populate.
/// @param logic_payload_data Pointer to LogicPayloadFbs flatbuffers data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureLogicPayload(LogicPayload &logic_payload,
                      const LogicPayloadFbs *logic_payload_data);

/////////////////////////////////////////////////
/// @brief Logic for populating ScenePayload from flatbuffers data source.
///
/// @param scene_payload Reference to ScenePayload to populate.
/// @param scene_payload_data Pointer to ScenePayloadFbs flatbuffers data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureScenePayload(ScenePayload &scene_payload,
                      const ScenePayloadFbs *scene_payload_data);

/////////////////////////////////////////////////
/// @brief Logic for populating SystemPayload from flatbuffers data source.
///
/// @param system_payload Reference to SystemPayload to populate.
/// @param system_payload_data Pointer to SystemPayloadFbs flatbuffers data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSystemPayload(SystemPayload &system_payload,
                       const SystemPayloadFbs *system_payload_data);

/////////////////////////////////////////////////
/// @brief Logic for populating GhostPayload from flatbuffers data source.
///
/// @param ghost_payload Reference to GhostPayload to populate.
/// @param ghost_payload_data Pointer to GhostPayloadFbs flatbuffers data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureGhostPayload(GhostPayload &ghost_payload,
                      const GhostPayloadFbs *ghost_payload_data);

/////////////////////////////////////////////////
/// @brief Logic for populating CameraPayload from flatbuffers data source.
///
/// @param camera_payload Reference to CameraPayload to populate.
/// @param camera_payload_data Pointer to CameraPayloadFbs flatbuffers data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureCameraPayload(CameraPayload &camera_payload,
                       const CameraPayloadFbs *camera_payload_data);

/////////////////////////////////////////////////
/// @brief Logic for populating EventType from flatbuffers data source.
///
/// @param event_type Reference to EventType to populate.
/// @param event_type_data EventTypeFbs flatbuffers enum.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEventType(EventType &event_type, EventTypeFbs event_type_data);

/////////////////////////////////////////////////
/// @brief Logic for populating EventPayload from flatbuffers data source.
///
/// @param event_payload Reference to EventPayload to populate.
/// @param event_payload_data Pointer to EventPayloadFbs flatbuffers union.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEventPayload(EventPayload &event_payload,
                      EventPayloadFbs event_payload_data,
                      const void *event_payload_ptr);

/////////////////////////////////////////////////
/// @brief Logic for populating EventPacket from flatbuffers data source.
///
/// @param event_packet Reference to EventPacket to populate.
/// @param event_packet_data Pointer to EventPacketFbs flatbuffers data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEventPacket(EventPacket &event_packet,
                     const EventPacketFbs *event_packet_data);

} // namespace steamrot::data::configure
