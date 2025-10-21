////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "CUserInterface.h"

#include "containers.h"

namespace steamrot {

/////////////////////////////////////////////////
size_t CUserInterface::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index =
      TupleTypeIndex<CUserInterface, ComponentRegister>;
  return index;
}

////////////////////////////////////////////////////////////
bool CUserInterface::operator==(const CUserInterface &other) const {
  // Compare base Component
  if (m_active != other.m_active)
    return false;

  // Compare name
  if (m_name != other.m_name)
    return false;

  // Compare UI visibility
  if (m_UI_visible != other.m_UI_visible)
    return false;

  // Compare root element (pointer-based comparison)
  // Both null or both non-null with same address
  if (m_root_element == nullptr && other.m_root_element == nullptr)
    return true;

  if (m_root_element == nullptr || other.m_root_element == nullptr)
    return false;

  // For polymorphic types, we compare pointers
  return m_root_element.get() == other.m_root_element.get();
}

} // namespace steamrot
