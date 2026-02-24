/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for converting SFML events.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "sfml_event_convert.h"
#include "UserInputBitset.h"
#include <bitset>

namespace steamrot::events::convert {

/////////////////////////////////////////////////
UserInputBitset
CollectInputEvents(const std::vector<sf::Event> &sfml_events) {
  return UserInputBitset(sfml_events);
}

/////////////////////////////////////////////////
std::optional<InputPayload::InputAction>
ResolveInputAction(const UserInputBitset &accumulated,
                   const InputActionRegistry &registry) {

  // Iterate the registry in order — first match wins.
  for (const auto &[pattern, action] : registry) {

    // Cast both sides to the base bitset so operator& is available.
    const auto &pat =
        static_cast<const std::bitset<kTotalBits> &>(pattern);
    const auto &acc =
        static_cast<const std::bitset<kTotalBits> &>(accumulated);

    // The pattern matches when every bit required by the pattern is
    // set in the accumulated bitset (subset / "contains" check).
    if ((pat & acc) == pat && pat.any()) {
      return action;
    }
  }

  return std::nullopt;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureInputAction(InputPayload::InputAction &action,
                     InputActionFbs action_fbs) {
  switch (action_fbs) {
  case InputActionFbs_SELECT:
    action = InputPayload::InputAction::SELECT;
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
ConfigureInputActionMapping(UserInputBitset &bitset,
                            InputPayload::InputAction &action,
                            const InputActionMappingFbs *mapping_data) {
  if (!mapping_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "InputActionMappingFbs data is null"});
  }

  // Populate keyboard-pressed bits.
  if (mapping_data->keyboard_pressed()) {
    for (auto key : *mapping_data->keyboard_pressed()) {
      bitset.setKeyPressed(static_cast<sf::Keyboard::Key>(key));
    }
  }

  // Populate keyboard-released bits.
  if (mapping_data->keyboard_released()) {
    for (auto key : *mapping_data->keyboard_released()) {
      bitset.setKeyReleased(static_cast<sf::Keyboard::Key>(key));
    }
  }

  // Populate mouse-pressed bits.
  if (mapping_data->mouse_pressed()) {
    for (auto button : *mapping_data->mouse_pressed()) {
      bitset.setMousePressed(static_cast<sf::Mouse::Button>(button));
    }
  }

  // Populate mouse-released bits.
  if (mapping_data->mouse_released()) {
    for (auto button : *mapping_data->mouse_released()) {
      bitset.setMouseReleased(static_cast<sf::Mouse::Button>(button));
    }
  }

  // Convert the InputActionFbs enum to the native InputAction via the
  // shared helper so the mapping only exists in one place.
  return ConfigureInputAction(action, mapping_data->action());
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureInputActionRegistry(InputActionRegistry &registry,
                             const InputActionConfigFbs *config_data) {
  if (!config_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "InputActionConfigFbs data is null"});
  }

  if (!config_data->mappings()) {
    // An empty config is valid — just results in an empty registry.
    return std::monostate{};
  }

  for (const auto *mapping_data : *config_data->mappings()) {
    UserInputBitset bitset;
    InputPayload::InputAction action{InputPayload::InputAction::NONE};

    auto result = ConfigureInputActionMapping(bitset, action, mapping_data);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }

    registry.emplace_back(bitset, action);
  }

  return std::monostate{};
}

} // namespace steamrot::events::convert
