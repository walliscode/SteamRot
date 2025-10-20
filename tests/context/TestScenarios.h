/////////////////////////////////////////////////
/// @file
/// @brief Declaration of TestScenarios class for reusable test setups
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "ArchetypeManager.h"
#include "ArchetypeUtils.h"
#include "CGrimoireMachina.h"
#include "CUserInterface.h"
#include "containers.h"
#include "entity_memory.h"
#include <cstddef>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class TestScenarios
/// @brief Provides pre-configured entity and component setups for testing
///
/// This class reduces test duplication by providing common entity configurations
/// that can be reused across different test scenarios.
/////////////////////////////////////////////////
class TestScenarios {

public:
  /////////////////////////////////////////////////
  /// @brief Create an empty EntityMemoryPool
  ///
  /// @return Empty EntityMemoryPool with default initialization
  /////////////////////////////////////////////////
  static EntityMemoryPool CreateEmptyPool();

  /////////////////////////////////////////////////
  /// @brief Create an EntityMemoryPool with N entities
  ///
  /// All entities created will have default-constructed components
  /// in all component vectors.
  ///
  /// @param n Number of entities to create
  /// @return EntityMemoryPool with n entities
  /////////////////////////////////////////////////
  static EntityMemoryPool CreatePoolWithNEntities(size_t n);

  /////////////////////////////////////////////////
  /// @brief Create an EntityMemoryPool with entities of a specific archetype
  ///
  /// Creates entities and activates components matching the given archetype ID.
  /// Non-matching components remain inactive (m_active = false).
  ///
  /// @param archetype_id The archetype ID for component activation
  /// @param n Number of entities to create
  /// @return EntityMemoryPool with entities configured for the archetype
  /////////////////////////////////////////////////
  static EntityMemoryPool CreatePoolWithArchetype(ArchetypeID archetype_id,
                                                   size_t n);

  /////////////////////////////////////////////////
  /// @brief Create an EntityMemoryPool with a single UI entity
  ///
  /// Creates one entity with an active CUserInterface component.
  /// Useful for UI-related tests.
  ///
  /// @return EntityMemoryPool with one UI entity
  /////////////////////////////////////////////////
  static EntityMemoryPool CreatePoolWithSingleUIEntity();

  /////////////////////////////////////////////////
  /// @brief Create an EntityMemoryPool with multiple UI entities
  ///
  /// Creates n entities each with an active CUserInterface component.
  /// Each entity has a unique position for testing spatial logic.
  ///
  /// @param n Number of UI entities to create
  /// @return EntityMemoryPool with n UI entities
  /////////////////////////////////////////////////
  static EntityMemoryPool CreatePoolWithMultipleUIEntities(size_t n);

  /////////////////////////////////////////////////
  /// @brief Create an EntityMemoryPool with a single Grimoire entity
  ///
  /// Creates one entity with an active CGrimoireMachina component.
  /// Useful for grimoire-specific tests.
  ///
  /// @return EntityMemoryPool with one grimoire entity
  /////////////////////////////////////////////////
  static EntityMemoryPool CreatePoolWithSingleGrimoireEntity();

  /////////////////////////////////////////////////
  /// @brief Activate a specific component for an entity
  ///
  /// Helper function to activate a component and set m_active = true.
  /// Useful for setting up test scenarios with specific component states.
  ///
  /// @tparam TComponent Type of component to activate
  /// @param entity_id ID of the entity
  /// @param pool EntityMemoryPool containing the entity
  /////////////////////////////////////////////////
  template <typename TComponent>
  static void ActivateComponent(size_t entity_id, EntityMemoryPool &pool);

  /////////////////////////////////////////////////
  /// @brief Deactivate a specific component for an entity
  ///
  /// Helper function to deactivate a component and set m_active = false.
  ///
  /// @tparam TComponent Type of component to deactivate
  /// @param entity_id ID of the entity
  /// @param pool EntityMemoryPool containing the entity
  /////////////////////////////////////////////////
  template <typename TComponent>
  static void DeactivateComponent(size_t entity_id, EntityMemoryPool &pool);

  /////////////////////////////////////////////////
  /// @brief Create an ArchetypeManager populated with entities
  ///
  /// Creates an ArchetypeManager and populates it with entities from the pool.
  /// This is useful for tests that need both an entity pool and archetype tracking.
  ///
  /// @param pool EntityMemoryPool to populate from
  /// @return Configured ArchetypeManager
  /////////////////////////////////////////////////
  static ArchetypeManager CreatePopulatedArchetypeManager(
      const EntityMemoryPool &pool);

private:
  /////////////////////////////////////////////////
  /// @brief Helper to activate component by its index in ComponentRegister
  ///
  /// @param entity_id ID of the entity
  /// @param component_index Index of component in ComponentRegister tuple
  /// @param pool EntityMemoryPool containing the entity
  /////////////////////////////////////////////////
  static void ActivateComponentByIndex(size_t entity_id,
                                       size_t component_index,
                                       EntityMemoryPool &pool);
};

/////////////////////////////////////////////////
// Template implementations
/////////////////////////////////////////////////

template <typename TComponent>
void TestScenarios::ActivateComponent(size_t entity_id,
                                      EntityMemoryPool &pool) {
  TComponent &component = entity::memory::GetComponent<TComponent>(entity_id, pool);
  component.m_active = true;
}

template <typename TComponent>
void TestScenarios::DeactivateComponent(size_t entity_id,
                                        EntityMemoryPool &pool) {
  TComponent &component = entity::memory::GetComponent<TComponent>(entity_id, pool);
  component.m_active = false;
}

} // namespace steamrot::tests
