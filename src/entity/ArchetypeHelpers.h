#pragma once

#include "containers.h"
#include <bitset>

using ArchetypeID = std::bitset<steamrot::kComponentRegisterSize>;

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Generates an ArchetypeID based on the provided Components.
///
/// @tparam Components Parameter pack of types derived from Component.
/// @return An ArchetypeID representing the combination of the provided
/// Components.
/////////////////////////////////////////////////
template <typename... Components> ArchetypeID GenerateArchetypeIDfromTypes() {
  ArchetypeID archetype_id;
  // Set the bits for the components in the ArchetypeID
  ((archetype_id.set(IndexOf<Components, ComponentRegister>::value)), ...);
  return archetype_id;
}
} // namespace steamrot
