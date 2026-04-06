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
#include <SFML/System/Vector2.hpp>
#include <magic_enum/magic_enum.hpp>
#include <random>

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
std::expected<EventPacket, FailInfo>
CreateSystemEventPacket(const uint8_t lifetime,
                        const SystemPayload::SystemAction action,
                        const sf::Vector2u resize_size) {
  // Construct a SystemPayload with resize data and reuse the core creation
  // logic from the base overload via the EventContext/EventPacket path.
  EventContext context{lifetime};
  SystemPayload payload(action, resize_size);
  EventPacket packet{context, EventType::SYSTEM, payload};
  return packet;
}

/////////////////////////////////////////////////
std::expected<EventPacket, FailInfo> CreateRandomEventPacket() {
  // Initialize random number generator
  std::random_device rd;
  std::mt19937 gen(rd());

  // Generate random lifetime (1 to max uint8_t)
  std::uniform_int_distribution<uint16_t> lifetime_dist(
      1, std::numeric_limits<uint8_t>::max());
  const uint8_t lifetime = static_cast<uint8_t>(lifetime_dist(gen));

  // Get all EventType enum values
  constexpr auto event_types = magic_enum::enum_values<EventType>();

  // Filter out NONE to get only valid event types
  std::vector<EventType> valid_types;
  for (const auto &type : event_types) {
    if (type != EventType::NONE) {
      valid_types.push_back(type);
    }
  }

  // Pick a random valid event type
  std::uniform_int_distribution<size_t> type_dist(0, valid_types.size() - 1);
  const EventType random_type = valid_types[type_dist(gen)];

  // Switch on the event type and create the appropriate payload
  switch (random_type) {
  case EventType::USER_INPUT: {
    // Get all InputAction values and filter out NONE
    constexpr auto input_actions =
        magic_enum::enum_values<InputPayload::InputAction>();
    std::vector<InputPayload::InputAction> valid_actions;
    for (const auto &action : input_actions) {
      if (action != InputPayload::InputAction::NONE) {
        valid_actions.push_back(action);
      }
    }
    std::uniform_int_distribution<size_t> action_dist(0,
                                                       valid_actions.size() - 1);
    const auto random_action = valid_actions[action_dist(gen)];
    return CreateInputEventPacket(lifetime, random_action);
  }

  case EventType::UI: {
    // Get all UIAction values
    constexpr auto ui_actions = magic_enum::enum_values<UIPayload::UIAction>();
    std::uniform_int_distribution<size_t> action_dist(0, ui_actions.size() - 1);
    const auto random_action = ui_actions[action_dist(gen)];

    // Generate random UI state name
    const std::string random_ui_name =
        "ui_state_" + std::to_string(lifetime_dist(gen));
    return CreateUIEventPacket(lifetime, random_action, random_ui_name);
  }

  case EventType::LOGIC: {
    // Get all LogicToggle values and filter out NONE
    constexpr auto logic_toggles =
        magic_enum::enum_values<LogicPayload::LogicToggle>();
    std::vector<LogicPayload::LogicToggle> valid_toggles;
    for (const auto &toggle : logic_toggles) {
      if (toggle != LogicPayload::LogicToggle::NONE) {
        valid_toggles.push_back(toggle);
      }
    }
    std::uniform_int_distribution<size_t> toggle_dist(
        0, valid_toggles.size() - 1);
    const auto random_toggle = valid_toggles[toggle_dist(gen)];
    return CreateLogicEventPacket(lifetime, random_toggle);
  }

  case EventType::SCENE: {
    // Get all SceneAction values and filter out NONE
    constexpr auto scene_actions =
        magic_enum::enum_values<ScenePayload::SceneAction>();
    std::vector<ScenePayload::SceneAction> valid_actions;
    for (const auto &action : scene_actions) {
      if (action != ScenePayload::SceneAction::NONE) {
        valid_actions.push_back(action);
      }
    }
    std::uniform_int_distribution<size_t> action_dist(0,
                                                       valid_actions.size() - 1);
    const auto random_action = valid_actions[action_dist(gen)];

    // Get all SceneType values and filter out UNKNOWN
    constexpr auto scene_types = magic_enum::enum_values<SceneType>();
    std::vector<SceneType> valid_types;
    for (const auto &type : scene_types) {
      if (type != SceneType::UNKNOWN) {
        valid_types.push_back(type);
      }
    }
    std::uniform_int_distribution<size_t> type_dist(0, valid_types.size() - 1);
    const auto random_scene_type = valid_types[type_dist(gen)];

    // Randomly decide whether to include scene_id (50% chance)
    std::uniform_int_distribution<int> bool_dist(0, 1);
    if (bool_dist(gen)) {
      auto scene_id = uuids::uuid_system_generator{}();
      return CreateSceneEventPacket(lifetime, random_action, random_scene_type,
                                    scene_id);
    } else {
      return CreateSceneEventPacket(lifetime, random_action, random_scene_type);
    }
  }

  case EventType::SYSTEM: {
    // Get all SystemAction values and filter out NONE and RESIZE
    // (RESIZE carries required size data and has a dedicated overload)
    constexpr auto system_actions =
        magic_enum::enum_values<SystemPayload::SystemAction>();
    std::vector<SystemPayload::SystemAction> valid_actions;
    for (const auto &action : system_actions) {
      if (action != SystemPayload::SystemAction::NONE &&
          action != SystemPayload::SystemAction::RESIZE) {
        valid_actions.push_back(action);
      }
    }
    std::uniform_int_distribution<size_t> action_dist(0,
                                                       valid_actions.size() - 1);
    const auto random_action = valid_actions[action_dist(gen)];
    return CreateSystemEventPacket(lifetime, random_action);
  }

  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "CreateRandomEventPacket: Invalid event type encountered"});
  }
}
} // namespace steamrot::events
