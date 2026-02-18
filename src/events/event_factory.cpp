/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the free functions for creating EventPackets
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_factory.h"
#include "EventContext.h"
#include "EventPayload.h"
#include "EventType.h"

namespace steamrot::events {

/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateInputEventPacket(const uint8_t lifetime,
                       const InputPayload::InputAction action) {
  EventContext context{lifetime};
  InputPayload payload(action);
  EventPacket packet{context, EventType::USER_INPUT, payload};
  return packet;
}

/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateUIEventPacket(const uint8_t lifetime, const UIPayload::UIAction action,
                    const std::string &c_ui_state_name) {
  EventContext context{lifetime};
  UIPayload payload(action, c_ui_state_name);
  EventPacket packet{context, EventType::UI, payload};
  return packet;
}

/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateLogicEventPacket(const uint8_t lifetime,
                       const LogicPayload::LogicToggle toggle_name) {
  EventContext context{lifetime};
  LogicPayload payload(toggle_name);
  EventPacket packet{context, EventType::LOGIC, payload};
  return packet;
}

/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateSceneEventPacket(const uint8_t lifetime,
                       const ScenePayload::SceneAction action,
                       const SceneType scene_type) {
  EventContext context{lifetime};
  ScenePayload payload(action, scene_type);
  EventPacket packet{context, EventType::SCENE, payload};
  return packet;
}

/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo> CreateSceneEventPacket(
    const uint8_t lifetime, const ScenePayload::SceneAction action,
    const SceneType scene_type, const uuids::uuid &scene_id) {
  EventContext context{lifetime};
  ScenePayload payload(action, scene_type, scene_id);
  EventPacket packet{context, EventType::SCENE, payload};
  return packet;
}

/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo>
CreateSystemEventPacket(const uint8_t lifetime,
                        const SystemPayload::SystemAction action) {
  EventContext context{lifetime};
  SystemPayload payload(action);
  EventPacket packet{context, EventType::SYSTEM, payload};
  return packet;
}

/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo> CreateRandomEventPacket() {
  // Use std::random_device and std::mt19937 for random generation
  std::random_device rd;
  std::mt19937 gen(rd());
}
} // namespace steamrot::events
