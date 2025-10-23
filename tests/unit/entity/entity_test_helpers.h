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
#include "CGrimoireMachina.h"
#include "CUserInterface.h"
#include "containers.h"
#include "grimoire_machina_generated.h"
#include "test_data_generated.h"
#include "user_interface_generated.h"

namespace steamrot {
class FlatbuffersConfigurator;
}

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
void TestConfigurationOfEMPfromDefaultData(
    const EntityMemoryPool &entity_memory_pool, const SceneType scene_type);

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

/////////////////////////////////////////////////
/// @brief Compares two EntityMemoryPool instances and fails if they don't match
///
/// @param actual The actual EntityMemoryPool to compare
/// @param expected The expected EntityMemoryPool to compare against
/////////////////////////////////////////////////
void CompareEntityMemoryPools(const EntityMemoryPool &actual,
                              const EntityMemoryPool &expected);

/////////////////////////////////////////////////
/// @brief Wrapper function to run EMP comparison tests based on TestDataConfig
///
/// This function examines the TestDataConfig and runs appropriate tests based
/// on what data is present:
/// - If start_entity_collection and expected_entity_collection are present,
///   creates both pools and compares them
/// - If only entity_collection is present, uses the old behavior
/// - If start_entity_collection is not present, starts with default-constructed pool
///
/// @param config The test data configuration to process
/// @param configurator FlatbuffersConfigurator instance for configuring pools
/////////////////////////////////////////////////
void RunEMPComparisonTest(const TestDataConfig *config,
                          FlatbuffersConfigurator &configurator);

} // namespace steamrot::tests
