////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of input-action configure free functions.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "configure_input_action.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace steamrot::data::configure {

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureInputAction(InputPayload::InputAction &action,
                     InputActionFbs action_fbs) {
  switch (action_fbs) {
  case InputActionFbs_SELECT:
    action = InputPayload::InputAction::SELECT;
    break;
  case InputActionFbs_TOGGLE_SOCKET_VISIBILITY:
    action = InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY;
    break;
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unknown InputActionFbs value in flatbuffers data"});
  }

  return std::monostate{};
}

////////////////////////////////////////////////////////////
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

  return ConfigureInputAction(action, mapping_data->action());
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureInputActionRegistry(InputActionRegistry &registry,
                             const InputActionConfigFbs *config_data) {
  if (!config_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "InputActionConfigFbs data is null"});
  }

  if (!config_data->mappings()) {
    return std::monostate{};
  }

  for (const auto *mapping_data : *config_data->mappings()) {
    UserInputBitset bitset;
    InputPayload::InputAction action{InputPayload::InputAction::NONE};

    auto result = ConfigureInputActionMapping(bitset, action, mapping_data);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }

    registry.insert_or_assign(bitset, action);
  }

  return std::monostate{};
}

} // namespace steamrot::data::configure
