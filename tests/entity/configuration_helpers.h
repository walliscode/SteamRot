/////////////////////////////////////////////////
/// @file
/// @brief definition of configuration helpers for entity tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "containers.h"
namespace steamrot {

/////////////////////////////////////////////////
/// @brief Check all components are constructed with default values
///
/// @param entity_memory_pool Instance of EntityMemoryPool to check
/////////////////////////////////////////////////
void TestEMPIsDefaultConstructed(const EntityMemoryPool &entity_memory_pool);

/////////////////////////////////////////////////
/// @brief Test configuration of EntityMemoryPool from default data
///
/// @param entity_memory_pool Instance of EntityMemoryPool to configure
/// @param scene_type SceneType to pull default data from
/////////////////////////////////////////////////
void TestConfigurationOfEMPfromDefaultData(EntityMemoryPool &entity_memory_pool,
                                           const SceneType scene_type);

} // namespace steamrot
