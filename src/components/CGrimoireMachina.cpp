/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CGrimoireMachina component
/////////////////////////////////////////////////

#include "CGrimoireMachina.h"
#include "containers.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot {

/////////////////////////////////////////////////
size_t CGrimoireMachina::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index =
      TupleTypeIndex<CGrimoireMachina, ComponentRegister>;
  return index;
}

////////////////////////////////////////////////////////////
bool CGrimoireMachina::operator==(const CGrimoireMachina &other) const {
  // Compare base Component
  if (m_active != other.m_active)
    return false;

  // Compare fragments
  if (m_all_fragments != other.m_all_fragments)
    return false;

  // Compare joints
  if (m_all_joints != other.m_all_joints)
    return false;

  // Compare machina forms
  if (m_machina_forms != other.m_machina_forms)
    return false;

  // Compare holding form (pointer-based comparison)
  if (m_holding_form == nullptr && other.m_holding_form == nullptr)
    return true;

  if (m_holding_form == nullptr || other.m_holding_form == nullptr)
    return false;

  // Compare the dereferenced values
  return *m_holding_form == *other.m_holding_form;
}

} // namespace steamrot
