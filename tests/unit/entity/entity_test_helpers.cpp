/////////////////////////////////////////////////
/// @file
/// @brief Implementation of configuration helpers for entity tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "entity_test_helpers.h"
#include "archetype_helpers.h"
#include "catch2/catch_test_macros.hpp"

namespace steamrot::tests {

/////////////////////////////////////////////////
void TestArchetypesOfUnconfiguredEMP(
    const std::unordered_map<ArchetypeID, Archetype> &archetypes) {
  // check that the archetypes map is empty
  REQUIRE(archetypes.empty());
  // check that the only archetype member is 0 and that it contains all the
  // entity indexes
  REQUIRE(archetypes.size() == 1);
  // check that the key is 0
  REQUIRE(archetypes.begin()->first == 0);
}

/////////////////////////////////////////////////
void TestArchetypesOfConfiguredEMPfromDefaultData(
    const std::unordered_map<ArchetypeID, Archetype> &archetypes,
    const SceneType scene_type) {}

} // namespace steamrot::tests
