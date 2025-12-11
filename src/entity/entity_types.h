/////////////////////////////////////////////////
/// @file
/// @brief typedefs for the entity library
/////////////////////////////////////////////////

#include "containers.h"
#include <bitset>
namespace steamrot {

//////////////////////////////////////////////////
/// @brief An Archetype is a vector of entity ids
///
/// Each archetype contains indexs of an EntityMemoryPool, these indexes refer
/// to entities that share the same set of components. This allows for efficient
/// selection of entities and minimal checking
using Archetype = std::vector<size_t>;

/////////////////////////////////////////////////
/// @brief An ArchetypeID is a bitset representing the components in an
/// archetype
///
/// Each bit in the bitset corresponds to a component type, where a bit value of
/// 1 indicates the presence of that component type in the archetype, and a bit
/// value of 0 indicates its absence.
using ArchetypeID = std::bitset<steamrot::ComponentRegisterSize>;
} // namespace steamrot
