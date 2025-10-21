/////////////////////////////////////////////////
/// @file
/// @brief Declaration of reusable test comparison functions
///
/// This file provides comparison utilities for verifying test data
/// states. These functions are designed to work with test data
/// loaded from FlatBuffers and enable data-driven testing.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "ArchetypeManager.h"
#include "Component.h"
#include "EventHandler.h"
#include "UIElement.h"
#include "containers.h"
#include "entity_memory.h"
#include <cstddef>
#include <string>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Result of a comparison operation
/////////////////////////////////////////////////
struct ComparisonResult {
  bool passed;
  std::string message;
  std::vector<std::string> differences;

  ComparisonResult(bool p = true) : passed(p) {}
  ComparisonResult(bool p, const std::string &msg)
      : passed(p), message(msg) {}
};

/////////////////////////////////////////////////
/// @brief Compare two components for equality
///
/// Template function to compare component values.
/// Specializations should be provided for each component type.
///
/// @tparam TComponent Type of component to compare
/// @param actual The actual component state
/// @param expected The expected component state
/// @return ComparisonResult with details of any differences
/////////////////////////////////////////////////
template <typename TComponent>
ComparisonResult CompareComponents(const TComponent &actual,
                                   const TComponent &expected);

/////////////////////////////////////////////////
/// @brief Compare two EntityMemoryPools
///
/// Compares all entities and their component states between two pools.
///
/// @param actual The actual EntityMemoryPool
/// @param expected The expected EntityMemoryPool
/// @return ComparisonResult with details of any differences
/////////////////////////////////////////////////
ComparisonResult CompareEntityMemoryPools(const EntityMemoryPool &actual,
                                          const EntityMemoryPool &expected);

/////////////////////////////////////////////////
/// @brief Compare two EventBus states
///
/// Compares the events in two event buses for equality.
///
/// @param actual The actual EventBus
/// @param expected The expected EventBus
/// @return ComparisonResult with details of any differences
/////////////////////////////////////////////////
ComparisonResult CompareEventBuses(const EventBus &actual,
                                   const EventBus &expected);

/////////////////////////////////////////////////
/// @brief Compare two ArchetypeManagers
///
/// Compares archetype structures and entity memberships.
///
/// @param actual The actual ArchetypeManager
/// @param expected The expected ArchetypeManager
/// @return ComparisonResult with details of any differences
/////////////////////////////////////////////////
ComparisonResult CompareArchetypeManagers(const ArchetypeManager &actual,
                                          const ArchetypeManager &expected);

/////////////////////////////////////////////////
/// @brief Compare component activation states across entities
///
/// Verifies that component activation states match expectations
/// for all entities in a range.
///
/// @tparam TComponent Type of component to check
/// @param pool The EntityMemoryPool to check
/// @param entity_count Number of entities to check
/// @param expected_active Expected activation state
/// @return ComparisonResult with details of any differences
/////////////////////////////////////////////////
template <typename TComponent>
ComparisonResult CompareComponentActivation(const EntityMemoryPool &pool,
                                            size_t entity_count,
                                            bool expected_active);

/////////////////////////////////////////////////
/// @brief Helper to create a comparison result with differences
///
/// @param differences Vector of difference descriptions
/// @return ComparisonResult marked as failed with listed differences
/////////////////////////////////////////////////
ComparisonResult CreateFailedComparison(
    const std::vector<std::string> &differences);

/////////////////////////////////////////////////
/// @brief Compare a subset of components in an EntityMemoryPool
///
/// Useful for partial state verification in tests.
///
/// @tparam TComponent Type of component to compare
/// @param actual The actual EntityMemoryPool
/// @param expected The expected EntityMemoryPool
/// @param entity_ids Vector of entity IDs to compare
/// @return ComparisonResult with details of any differences
/////////////////////////////////////////////////
template <typename TComponent>
ComparisonResult
CompareComponentsForEntities(const EntityMemoryPool &actual,
                             const EntityMemoryPool &expected,
                             const std::vector<size_t> &entity_ids);

} // namespace steamrot::tests
