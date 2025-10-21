////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "CUIState.h"

#include "containers.h"

namespace steamrot {

////////////////////////////////////////////////////////////
size_t CUIState::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index = TupleTypeIndex<CUIState, ComponentRegister>;
  return index;
}

////////////////////////////////////////////////////////////
bool CUIState::operator==(const CUIState &other) const {
  // Compare base Component
  if (m_active != other.m_active)
    return false;

  // Compare state to ui visibility
  if (m_state_to_ui_visibility != other.m_state_to_ui_visibility)
    return false;

  // Compare state values
  if (m_state_values != other.m_state_values)
    return false;

  // Compare state subscribers (pointer-based comparison)
  if (m_state_subscribers.size() != other.m_state_subscribers.size())
    return false;

  for (const auto &[key, subscribers] : m_state_subscribers) {
    auto it = other.m_state_subscribers.find(key);
    if (it == other.m_state_subscribers.end())
      return false;

    const auto &other_subscribers = it->second;
    if (subscribers.size() != other_subscribers.size())
      return false;

    // Compare subscriber pointers
    for (size_t i = 0; i < subscribers.size(); ++i) {
      if (subscribers[i].get() != other_subscribers[i].get())
        return false;
    }
  }

  return true;
}

} // namespace steamrot
