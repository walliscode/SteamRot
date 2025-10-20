/////////////////////////////////////////////////
/// @file
/// @brief Declaration of reusable test assertion functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "ArchetypeManager.h"
#include "Component.h"
#include "UIElement.h"
#include "containers.h"
#include "entity_memory.h"
#include <cstddef>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Verify that a component is properly configured
///
/// Checks that the component's m_active flag matches expectations
/// and that basic component properties are set correctly.
///
/// @param component The component to verify
/// @param should_be_active Expected activation state
/// @param test_name Name of the test for error messages
/////////////////////////////////////////////////
void AssertComponentActive(const Component &component, bool should_be_active,
                          const std::string &test_name = "");

/////////////////////////////////////////////////
/// @brief Verify that an entity belongs to a specific archetype
///
/// Checks that the entity ID exists in the archetype's entity list.
///
/// @param entity_id The entity ID to check
/// @param archetype_id The archetype to verify membership in
/// @param manager The ArchetypeManager to query
/// @param test_name Name of the test for error messages
/////////////////////////////////////////////////
void AssertEntityInArchetype(size_t entity_id, ArchetypeID archetype_id,
                             const ArchetypeManager &manager,
                             const std::string &test_name = "");

/////////////////////////////////////////////////
/// @brief Verify that an archetype exists in the manager
///
/// Checks that the archetype ID is present in the archetype map.
///
/// @param archetype_id The archetype to verify existence of
/// @param manager The ArchetypeManager to query
/// @param test_name Name of the test for error messages
/////////////////////////////////////////////////
void AssertArchetypeExists(ArchetypeID archetype_id,
                          const ArchetypeManager &manager,
                          const std::string &test_name = "");

/////////////////////////////////////////////////
/// @brief Verify that an archetype has the expected number of entities
///
/// @param archetype_id The archetype to check
/// @param expected_count Expected number of entities
/// @param manager The ArchetypeManager to query
/// @param test_name Name of the test for error messages
/////////////////////////////////////////////////
void AssertArchetypeEntityCount(ArchetypeID archetype_id,
                               size_t expected_count,
                               const ArchetypeManager &manager,
                               const std::string &test_name = "");

/////////////////////////////////////////////////
/// @brief Verify that a component has specific values
///
/// Template function to check component-specific properties.
/// Specializations should be provided for each component type.
///
/// @tparam TComponent Type of component to verify
/// @param component The component to verify
/// @param expected_value Expected value to compare against
/// @param test_name Name of the test for error messages
/////////////////////////////////////////////////
template <typename TComponent, typename TExpected>
void AssertComponentValue(const TComponent &component,
                         const TExpected &expected_value,
                         const std::string &test_name = "");

/////////////////////////////////////////////////
/// @brief Verify that all entities in a pool have valid indices
///
/// Checks that entity indices are sequential and within bounds.
///
/// @param pool The EntityMemoryPool to verify
/// @param test_name Name of the test for error messages
/////////////////////////////////////////////////
void AssertEntityPoolValid(const EntityMemoryPool &pool,
                          const std::string &test_name = "");

/////////////////////////////////////////////////
/// @brief Verify that a component is default-constructed
///
/// Checks that all component fields have their default values.
///
/// @tparam TComponent Type of component to verify
/// @param component The component to check
/// @param test_name Name of the test for error messages
/////////////////////////////////////////////////
template <typename TComponent>
void AssertComponentDefaultConstructed(const TComponent &component,
                                      const std::string &test_name = "");

} // namespace steamrot::tests
