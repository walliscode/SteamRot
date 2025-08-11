/////////////////////////////////////////////////
/// @file
/// @brief definition of configuration helpers for entity tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "ArchetypeHelpers.h"
#include "ArchetypeManager.h"
#include "CGrimoireMachina.h"
#include "CUserInterface.h"
#include "containers.h"
#include "grimoire_machina_generated.h"
#include "user_interface_generated.h"
namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Helper function to compare two CUserInterface instances
///
/// @param actual Instance of CUserInterface to compare against baseline
/////////////////////////////////////////////////
void CompareToDefault(const CUserInterface &actual);

/////////////////////////////////////////////////
/// @brief Overloaded helper function to compare CGrimoireMachine to default
/// values
///
/// @param actual Instance of CGrimoireMachine to compare against baseline
/////////////////////////////////////////////////
void CompareToDefault(const CGrimoireMachina &actual);

/////////////////////////////////////////////////
/// @brief Check all components are constructed with default values
///
/// @param entity_memory_pool Instance of EntityMemoryPool to check
/////////////////////////////////////////////////
void TestEMPIsDefaultConstructed(const EntityMemoryPool &entity_memory_pool);

/////////////////////////////////////////////////
/// @brief Compares a configured Component to a Data source
///
/// @param actual Instance of Component
/// @param data Data source from which Component should have been configured
/////////////////////////////////////////////////
void CompareToData(const CUserInterface &actual, const UserInterfaceData &data);

/////////////////////////////////////////////////
/// @brief Compares a configured Component to a Data source
///
/// @param actual Instance of Component
/// @param data Data source from which Component should have been configured
/////////////////////////////////////////////////
void CompareToData(const CGrimoireMachina &actual,
                   const GrimoireMachinaData &data);
/////////////////////////////////////////////////
/// @brief Test configuration of EntityMemoryPool from default data
///
/// @param entity_memory_pool Instance of EntityMemoryPool to configure
/// @param scene_type SceneType to pull default data from
/////////////////////////////////////////////////
void TestConfigurationOfEMPfromDefaultData(EntityMemoryPool &entity_memory_pool,
                                           const SceneType scene_type);

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
