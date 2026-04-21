/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for converting SFML events.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "sfml_event_convert.h"
#include "InputActionRegistry.h"
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

  for (const auto &[pattern, action] : registry) {

    const auto &pat =
        static_cast<const std::bitset<kTotalBits> &>(pattern);
    const auto &acc =
        static_cast<const std::bitset<kTotalBits> &>(accumulated);

    if ((pat & acc) == pat && pat.any()) {
      return action;
    }
  }

  return std::nullopt;
}

/////////////////////////////////////////////////
bool CollectSystemEvents(const std::vector<sf::Event> &sfml_events) {
  for (const auto &event : sfml_events) {
    if (event.is<sf::Event::Closed>()) {
      return true;
    }
    if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
      if (key_pressed->code == sf::Keyboard::Key::Escape) {
        return true;
      }
    }
  }
  return false;
}

/////////////////////////////////////////////////
bool CollectGhostClearEvents(const std::vector<sf::Event> &sfml_events) {
  for (const auto &event : sfml_events) {
    if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
      if (key_pressed->code == sf::Keyboard::Key::Q) {
        return true;
      }
    }
  }
  return false;
}

/////////////////////////////////////////////////
std::optional<float>
CollectScrollDelta(const std::vector<sf::Event> &sfml_events) {
  float total_delta = 0.0f;
  bool found = false;

  for (const auto &event : sfml_events) {
    if (const auto *scroll =
            event.getIf<sf::Event::MouseWheelScrolled>()) {
      total_delta += scroll->delta;
      found = true;
    }
  }

  if (!found)
    return std::nullopt;

  return total_delta;
}

/////////////////////////////////////////////////
std::vector<CameraPayload>
CollectPanEvents(const std::vector<sf::Event> &sfml_events) {
  std::vector<CameraPayload> result;

  for (const auto &event : sfml_events) {
    if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
      switch (key_pressed->code) {
      case sf::Keyboard::Key::W:
        result.emplace_back(CameraPayload::CameraAction::PAN_PRESS,
                            CameraPayload::PanDirection::UP);
        break;
      case sf::Keyboard::Key::A:
        result.emplace_back(CameraPayload::CameraAction::PAN_PRESS,
                            CameraPayload::PanDirection::LEFT);
        break;
      case sf::Keyboard::Key::S:
        result.emplace_back(CameraPayload::CameraAction::PAN_PRESS,
                            CameraPayload::PanDirection::DOWN);
        break;
      case sf::Keyboard::Key::D:
        result.emplace_back(CameraPayload::CameraAction::PAN_PRESS,
                            CameraPayload::PanDirection::RIGHT);
        break;
      default:
        break;
      }
    }

    if (const auto *key_released = event.getIf<sf::Event::KeyReleased>()) {
      switch (key_released->code) {
      case sf::Keyboard::Key::W:
        result.emplace_back(CameraPayload::CameraAction::PAN_RELEASE,
                            CameraPayload::PanDirection::UP);
        break;
      case sf::Keyboard::Key::A:
        result.emplace_back(CameraPayload::CameraAction::PAN_RELEASE,
                            CameraPayload::PanDirection::LEFT);
        break;
      case sf::Keyboard::Key::S:
        result.emplace_back(CameraPayload::CameraAction::PAN_RELEASE,
                            CameraPayload::PanDirection::DOWN);
        break;
      case sf::Keyboard::Key::D:
        result.emplace_back(CameraPayload::CameraAction::PAN_RELEASE,
                            CameraPayload::PanDirection::RIGHT);
        break;
      default:
        break;
      }
    }
  }

  return result;
}

} // namespace steamrot::events::convert
