/////////////////////////////////////////////////
/// @file
/// @brief typedefs for the entity library
/////////////////////////////////////////////////

#include "containers.h"
#include <bitset>
namespace steamrot {

using Archetype = std::vector<size_t>;

using ArchetypeID = std::bitset<steamrot::kComponentRegisterSize>;
} // namespace steamrot
