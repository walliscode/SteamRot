/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for configuring input-action data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_input_action.h"
#include "configure_event.h"
#include "UserInputBitset.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace steamrot::data::configure {

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
std::expected<std::monostate, FailInfo> ConfigureInputActionRegistry(
    SFMLEventConverter::InputActionRegistry &registry,
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

} // namespace steamrot::data::configure
