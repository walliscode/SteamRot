/////////////////////////////////////////////////
/// @file
/// @brief definition of configuration helpers for entity tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "ArchetypeUtils.h"
#include "ArchetypeManager.h"
#include "containers.h"

namespace steamrot {
class FlatbuffersConfigurator;
}

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Checks the archetypes of an unconfigured EntityMemoryPool
///
/// @param archetypes Map of ArchetypeID to Archetype
/////////////////////////////////////////////////
void TestArchetypesOfUnconfiguredEMP(
    const std::unordered_map<ArchetypeID, Archetype> &archetypes);

/////////////////////////////////////////////////
/// @brief Loads the default data and tests that the archetypes match the
/// provided data
///
/// @param archetypes generated archetypes from the EntityMemoryPool
/// @param scene_type SceneType to pull default data from
/////////////////////////////////////////////////
void TestArchetypesOfConfiguredEMPfromDefaultData(
    const std::unordered_map<ArchetypeID, Archetype> &archetypes,
    const SceneType scene_type);

} // namespace steamrot::tests
