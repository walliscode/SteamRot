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

/////////////////////////////////////////////////
void CGrimoireMachina::Configure(const GrimoireMachinaData *grimoire_data) {

  // Set component level data
  m_active = true;
}

} // namespace steamrot
