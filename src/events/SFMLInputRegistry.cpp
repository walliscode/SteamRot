/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SFMLInputRegistry class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SFMLInputRegistry.h"
#include "event_factory.h"

namespace steamrot {

/////////////////////////////////////////////////
void SFMLInputRegistry::Configure(const std::vector<SFMLInputBinding> &bindings) {
  m_bindings = bindings;
  m_held_keys.clear();
  m_held_buttons.clear();
  m_just_released_keys.clear();
  m_just_released_buttons.clear();
  m_active_binding_indices.clear();
}

/////////////////////////////////////////////////
bool SFMLInputRegistry::IsBindingSatisfied(
    const SFMLInputBinding &binding) const {

  if (binding.required_inputs.empty())
    return false;

  for (const auto &entry : binding.required_inputs) {
    if (entry.type == SFMLInputEntry::Type::Keyboard) {
      if (entry.trigger_on == SFMLInputEntry::TriggerOn::Pressed) {
        if (!m_held_keys.contains(entry.keyboard_key))
          return false;
      } else {
        // Released — satisfied only when the key was released this event
        if (!m_just_released_keys.contains(entry.keyboard_key))
          return false;
      }
    } else {
      if (entry.trigger_on == SFMLInputEntry::TriggerOn::Pressed) {
        if (!m_held_buttons.contains(entry.mouse_button))
          return false;
      } else {
        if (!m_just_released_buttons.contains(entry.mouse_button))
          return false;
      }
    }
  }
  return true;
}

/////////////////////////////////////////////////
std::vector<EventPacket> SFMLInputRegistry::CheckBindings() {
  std::vector<EventPacket> triggered_packets;

  for (size_t i = 0; i < m_bindings.size(); ++i) {
    const auto &binding = m_bindings[i];
    const bool was_active = m_active_binding_indices.contains(i);
    const bool is_now_active = IsBindingSatisfied(binding);

    if (!was_active && is_now_active) {
      // Binding just became satisfied — fire
      m_active_binding_indices.insert(i);
      auto result = events::CreateInputEventPacket(1, binding.action);
      if (result.has_value())
        triggered_packets.push_back(result.value());
    } else if (was_active && !is_now_active) {
      // Binding is no longer satisfied
      m_active_binding_indices.erase(i);
    }
  }

  return triggered_packets;
}

/////////////////////////////////////////////////
std::vector<EventPacket>
SFMLInputRegistry::ProcessSFMLEvent(const sf::Event &event) {

  // Clear per-event released state
  m_just_released_keys.clear();
  m_just_released_buttons.clear();

  if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
    m_held_keys.insert(key_pressed->code);
  } else if (const auto *key_released = event.getIf<sf::Event::KeyReleased>()) {
    m_held_keys.erase(key_released->code);
    m_just_released_keys.insert(key_released->code);
  } else if (const auto *mouse_pressed =
                 event.getIf<sf::Event::MouseButtonPressed>()) {
    m_held_buttons.insert(mouse_pressed->button);
  } else if (const auto *mouse_released =
                 event.getIf<sf::Event::MouseButtonReleased>()) {
    m_held_buttons.erase(mouse_released->button);
    m_just_released_buttons.insert(mouse_released->button);
  } else {
    // Not a key or button event — no bindings to evaluate
    return {};
  }

  return CheckBindings();
}

/////////////////////////////////////////////////
const std::vector<SFMLInputBinding> &SFMLInputRegistry::GetBindings() const {
  return m_bindings;
}

/////////////////////////////////////////////////
const std::set<sf::Keyboard::Key> &SFMLInputRegistry::GetHeldKeys() const {
  return m_held_keys;
}

/////////////////////////////////////////////////
const std::set<sf::Mouse::Button> &SFMLInputRegistry::GetHeldButtons() const {
  return m_held_buttons;
}

} // namespace steamrot
