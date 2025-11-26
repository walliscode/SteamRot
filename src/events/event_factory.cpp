/////////////////////////////////////////////////
/// @file
/// @brief Implementation of EventData factory functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_factory.h"
#include "EventPacket.h"
#include "events_generated.h"
#include "uuid.h"
#include <cstddef>
#include <unordered_map>
#include <variant>

namespace steamrot::event {

/////////////////////////////////////////////////
static const std::unordered_map<KeyboardInput, sf::Keyboard::Key> &
GetFlatbuffersToSFMLKeyboardMap() {
  static const std::unordered_map<KeyboardInput, sf::Keyboard::Key>
      string_to_key_map = {
          {KeyboardInput_A, sf::Keyboard::Key::A},
          {KeyboardInput_B, sf::Keyboard::Key::B},
          {KeyboardInput_C, sf::Keyboard::Key::C},
          {KeyboardInput_D, sf::Keyboard::Key::D},
          {KeyboardInput_E, sf::Keyboard::Key::E},
          {KeyboardInput_F, sf::Keyboard::Key::F},
          {KeyboardInput_G, sf::Keyboard::Key::G},
          {KeyboardInput_H, sf::Keyboard::Key::H},
          {KeyboardInput_I, sf::Keyboard::Key::I},
          {KeyboardInput_J, sf::Keyboard::Key::J},
          {KeyboardInput_K, sf::Keyboard::Key::K},
          {KeyboardInput_L, sf::Keyboard::Key::L},
          {KeyboardInput_M, sf::Keyboard::Key::M},
          {KeyboardInput_N, sf::Keyboard::Key::N},
          {KeyboardInput_O, sf::Keyboard::Key::O},
          {KeyboardInput_P, sf::Keyboard::Key::P},
          {KeyboardInput_Q, sf::Keyboard::Key::Q},
          {KeyboardInput_R, sf::Keyboard::Key::R},
          {KeyboardInput_S, sf::Keyboard::Key::S},
          {KeyboardInput_T, sf::Keyboard::Key::T},
          {KeyboardInput_U, sf::Keyboard::Key::U},
          {KeyboardInput_V, sf::Keyboard::Key::V},
          {KeyboardInput_W, sf::Keyboard::Key::W},
          {KeyboardInput_X, sf::Keyboard::Key::X},
          {KeyboardInput_Y, sf::Keyboard::Key::Y},
          {KeyboardInput_Z, sf::Keyboard::Key::Z},
      };
  return string_to_key_map;
}

/////////////////////////////////////////////////
static const std::unordered_map<MouseInput, sf::Mouse::Button> &
GetFlatbuffersToSFMLMouseMap() {
  static const std::unordered_map<MouseInput, sf::Mouse::Button>
      string_to_mouse_map = {

          {MouseInput_LEFT_CLICK, sf::Mouse::Button::Left},
          {MouseInput_RIGHT_CLICK, sf::Mouse::Button::Right},
          {MouseInput_MIDDLE_CLICK, sf::Mouse::Button::Middle},

      };
  return string_to_mouse_map;
};

/////////////////////////////////////////////////
std::expected<UserInputBitset, FailInfo>
CreateUserInputBitset(const UserInputBitsetData &data) {

  // create EventBitset that represents the keys for this action
  UserInputBitset event_bitset;
  if (data.keyboard_pressed()) {
    for (const auto &key_pressed : *data.keyboard_pressed()) {
      // not sure how to pull the enum straight out so just recasting it
      KeyboardInput key_pressed_cast = KeyboardInput(key_pressed);
      size_t bit_position = static_cast<size_t>(
          GetFlatbuffersToSFMLKeyboardMap().at(key_pressed_cast));

      // add to bitset (not replace)
      event_bitset.set(bit_position, true);
    }
  }
  if (data.keyboard_released()) {
    for (const auto &key_released : *data.keyboard_released()) {
      // not sure how to pull the enum straight out so just recasting it
      KeyboardInput key_released_cast = KeyboardInput(key_released);
      size_t bit_position = static_cast<size_t>(
          GetFlatbuffersToSFMLKeyboardMap().at(key_released_cast));
      // add to bitset (not replace)
      event_bitset.set(bit_position + sf::Keyboard::KeyCount, true);
    }
  }
  if (data.mouse_pressed()) {
    for (const auto &mouse_pressed : *data.mouse_pressed()) {
      // not sure how to pull the enum straight out so just recasting it
      MouseInput mouse_pressed_cast = MouseInput(mouse_pressed);
      size_t bit_position = static_cast<size_t>(
          GetFlatbuffersToSFMLMouseMap().at(mouse_pressed_cast));
      // add to bitset (not replace)
      event_bitset.set(bit_position + sf::Keyboard::KeyCount * 2, true);
    }
  }
  if (data.mouse_released()) {
    for (const auto &mouse_released : *data.mouse_released()) {
      // not sure how to pull the enum straight out so just recasting it
      MouseInput mouse_released_cast = MouseInput(mouse_released);
      size_t bit_position = static_cast<size_t>(
          GetFlatbuffersToSFMLMouseMap().at(mouse_released_cast));
      // add to bitset (not replace)
      event_bitset.set(bit_position + sf::Keyboard::KeyCount * 2 +
                           sf::Mouse::ButtonCount,
                       true);
    }
  }

  return event_bitset;
}

/////////////////////////////////////////////////
std::expected<SceneChangePacket, FailInfo>
CreateSceneChangePacket(const SceneChangePacketData &data) {

  SceneChangePacket scene_change_packet;

  if (data.uuid()) {

    // check if uuid is valid
    if (!uuids::uuid::is_valid_uuid(data.uuid()->c_str())) {
      return std::unexpected(
          FailInfo{FailMode::InvalidUUID,
                   "CreateSceneChangePacket: SceneChangePacketData "
                   "contains invalid UUID."});
    }

    scene_change_packet.first =
        uuids::uuid::from_string(data.uuid()->c_str());
  }
  if (data.scene_type() == SceneType_UNKNOWN) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "CreateSceneChangePacket: SceneChangePacketData "
                 "missing scene_type."});
  }
  scene_change_packet.second = data.scene_type();

  return scene_change_packet;
}

/////////////////////////////////////////////////
std::expected<UserInterfaceName, FailInfo>
CreateUserInterfaceName(const UserInterfaceNameData &data) {

  // check if name exists
  if (!data.id()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "CreateUserInterfaceName: UserInterfaceNameData "
                 "missing id."});
  }

  UserInterfaceName ui_name{data.id()->str()};

  return ui_name;
}

/////////////////////////////////////////////////
std::expected<EventData, FailInfo>
CreateEventData(const EventDataData data_type, const void *data) {

  switch (data_type) {
  case EventDataData::EventDataData_UserInputBitsetData: {

    // cast data to UserInputBitsetData
    auto user_input_bitset_data =
        static_cast<const UserInputBitsetData *>(data);

    // convert to UserInputBitset
    auto user_input_bitset_result =
        CreateUserInputBitset(*user_input_bitset_data);
    if (!user_input_bitset_result.has_value())
      return std::unexpected(user_input_bitset_result.error());

    return user_input_bitset_result.value();
  }
  case EventDataData::EventDataData_SceneChangePacketData: {
    // cast data to SceneChangePacketData
    auto scene_change_packet_data =
        static_cast<const SceneChangePacketData *>(data);

    // convert to SceneChangePacket
    auto scene_change_packet_result =
        CreateSceneChangePacket(*scene_change_packet_data);
    if (!scene_change_packet_result.has_value())
      return std::unexpected(scene_change_packet_result.error());

    return scene_change_packet_result.value();
  }

  case EventDataData::EventDataData_UserInterfaceNameData: {

    // cast data to UserInterfaceNameData
    auto ui_name_data = static_cast<const UserInterfaceNameData *>(data);

    // convert to UserInterfaceName
    auto ui_name_result = CreateUserInterfaceName(*ui_name_data);
    if (!ui_name_result.has_value())
      return std::unexpected(ui_name_result.error());

    return ui_name_result.value();
  }

  case EventDataData::EventDataData_NONE: {
    return std::monostate();
  }

  default:
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled,
                 "CreateEventData: EventDataData type not handled."});
  }
}

} // namespace steamrot::event
