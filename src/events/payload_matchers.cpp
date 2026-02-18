/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for matching payloads of events.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "payload_matchers.h"
#include <variant>

namespace steamrot::events {
/////////////////////////////////////////////////
bool MatchPayload(const InputPayload &filter_payload,
                  const InputPayload &event_payload) {

  if (filter_payload.action != event_payload.action) {
    return false;
  }
  return true;
}

/////////////////////////////////////////////////
bool MatchPayload(const UIPayload &filter_payload,
                  const UIPayload &event_payload) {

  if (filter_payload.action != event_payload.action) {
    return false;
  }

  if (filter_payload.c_ui_state_name != event_payload.c_ui_state_name) {
    return false;
  }

  return true;
}

/////////////////////////////////////////////////
bool MatchPayload(const LogicPayload &filter_payload,
                  const LogicPayload &event_payload) {

  if (filter_payload.toggle_name != event_payload.toggle_name) {
    return false;
  }

  return true;
}

/////////////////////////////////////////////////
bool MatchPayload(const ScenePayload &filter_payload,
                  const ScenePayload &event_payload) {

  if (filter_payload.action != event_payload.action) {
    return false;
  }

  // Currently only Scene actions are needed for checking, the rest is just
  // specific data

  // if (filter_payload.scene_type != event_payload.scene_type) {
  //   return false;
  // }
  //
  // // Check scene_id if both have values
  // if (filter_payload.scene_id.has_value() &&
  //     event_payload.scene_id.has_value()) {
  //   if (filter_payload.scene_id.value() != event_payload.scene_id.value()) {
  //     return false;
  //   }
  // } else if (filter_payload.scene_id.has_value() !=
  //            event_payload.scene_id.has_value()) {
  //   // If one has value and other doesn't, they don't match
  //   return false;
  // }

  return true;
}

/////////////////////////////////////////////////
bool MatchPayload(const SystemPayload &filter_payload,
                  const SystemPayload &event_payload) {

  if (filter_payload.action != event_payload.action) {
    return false;
  }

  return true;
}

/////////////////////////////////////////////////
bool MatchEventPacket(const EventPacket &filter_packet,
                      const EventPacket &event_packet) {

  // Check if variant types match
  if (filter_packet.payload.index() != event_packet.payload.index()) {
    return false;
  }

  // Check EventType match
  if (filter_packet.type != event_packet.type) {
    return false;
  }

  // Match based on the specific payload variant
  if (std::holds_alternative<InputPayload>(filter_packet.payload)) {
    return MatchPayload(std::get<InputPayload>(filter_packet.payload),
                        std::get<InputPayload>(event_packet.payload));
  } else if (std::holds_alternative<UIPayload>(filter_packet.payload)) {
    return MatchPayload(std::get<UIPayload>(filter_packet.payload),
                        std::get<UIPayload>(event_packet.payload));
  } else if (std::holds_alternative<LogicPayload>(filter_packet.payload)) {
    return MatchPayload(std::get<LogicPayload>(filter_packet.payload),
                        std::get<LogicPayload>(event_packet.payload));
  } else if (std::holds_alternative<ScenePayload>(filter_packet.payload)) {
    return MatchPayload(std::get<ScenePayload>(filter_packet.payload),
                        std::get<ScenePayload>(event_packet.payload));
  } else if (std::holds_alternative<SystemPayload>(filter_packet.payload)) {
    return MatchPayload(std::get<SystemPayload>(filter_packet.payload),
                        std::get<SystemPayload>(event_packet.payload));
  }

  return false;
}

} // namespace steamrot::events
