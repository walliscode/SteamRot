/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the free functions for creating EventPackets
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "EventPacket.h"
#include "FailInfo.h"
#include <expected>

namespace steamrot::events {

/////////////////////////////////////////////////
/// @brief Creates an EventPacket with an InputPayload
///
/// @param lifetime The lifetime of the event in ticks
/// @param action The input action to be performed
/// @return EventPacket with InputPayload or FailInfo on error
/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateInputEventPacket(const uint8_t lifetime,
                       const InputPayload::InputAction action);

/////////////////////////////////////////////////
/// @brief Creates an EventPacket with a UIPayload
///
/// @param lifetime The lifetime of the event in ticks
/// @param action The UI action to be performed
/// @param c_ui_state_name The name of the UI state being targeted
/// @return EventPacket with UIPayload or FailInfo on error
/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateUIEventPacket(const uint8_t lifetime, const UIPayload::UIAction action,
                    const std::string &c_ui_state_name);

/////////////////////////////////////////////////
/// @brief Creates an EventPacket with a LogicPayload
///
/// @param lifetime The lifetime of the event in ticks
/// @param toggle_name The logic toggle to be activated
/// @return EventPacket with LogicPayload or FailInfo on error
/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateLogicEventPacket(const uint8_t lifetime,
                       const LogicPayload::LogicToggle toggle_name);

/////////////////////////////////////////////////
/// @brief Creates an EventPacket with a ScenePayload (without scene_id)
///
/// @param lifetime The lifetime of the event in ticks
/// @param action The scene action to be performed
/// @param scene_type The type of scene being targeted
/// @return EventPacket with ScenePayload or FailInfo on error
/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateSceneEventPacket(const uint8_t lifetime,
                       const ScenePayload::SceneAction action,
                       const SceneType scene_type);

/////////////////////////////////////////////////
/// @brief Creates an EventPacket with a ScenePayload (with scene_id)
///
/// @param lifetime The lifetime of the event in ticks
/// @param action The scene action to be performed
/// @param scene_type The type of scene being targeted
/// @param scene_id The unique identifier for the specific scene instance
/// @return EventPacket with ScenePayload or FailInfo on error
/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateSceneEventPacket(const uint8_t lifetime,
                       const ScenePayload::SceneAction action,
                       const SceneType scene_type, const uuids::uuid &scene_id);

/////////////////////////////////////////////////
/// @brief Creates an EventPacket with a SystemPayload
///
/// @param lifetime The lifetime of the event in ticks
/// @param action The system action to be performed
/// @return EventPacket with SystemPayload or FailInfo on error
/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateSystemEventPacket(const uint8_t lifetime,
                        const SystemPayload::SystemAction action);

/////////////////////////////////////////////////
/// @brief Creates an EventPacket with a GhostPayload
///
/// @param lifetime   The lifetime of the event in ticks
/// @param action     The ghost action to be performed
/// @param selection  The GhostSelection tag identifying the chosen item
/// @return EventPacket with GhostPayload or FailInfo on error
/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateGhostEventPacket(const uint8_t lifetime,
                       const GhostPayload::GhostAction action,
                       const GhostSelection &selection);

/////////////////////////////////////////////////
/// @brief Creates an EventPacket with a CameraPayload carrying a scroll delta.
///
/// @param lifetime     The lifetime of the event in ticks.
/// @param scroll_delta Accumulated mouse-wheel delta for this tick.
/// @return EventPacket with CameraPayload or FailInfo on error.
/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateCameraEventPacket(uint8_t lifetime, float scroll_delta);

/////////////////////////////////////////////////
/// @brief Creates an EventPacket with random valid values for testing purposes
/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo> CreateRandomEventPacket();
} // namespace steamrot::events
