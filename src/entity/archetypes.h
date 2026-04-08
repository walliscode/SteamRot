#pragma once

#include "containers.h"
#include "entity_memory.h"
#include "entity_types.h"
#include <algorithm>
#include <bitset>
#include <cstddef>
#include <set>
#include <unordered_map>
#include <vector>

namespace steamrot::archetypes {

/////////////////////////////////////////////////
// typedefs
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Generates an ArchetypeID based on the provided Components.
///
/// @tparam Components Parameter pack of types derived from Component.
/// @return An ArchetypeID representing the combination of the provided
/// Components.
/////////////////////////////////////////////////
template <typename... Components> ArchetypeID GenerateArchetypeIDfromTypes() {
  ArchetypeID archetype_id;
  // Set the bits for the components in the ArchetypeID
  ((archetype_id.set(IndexOf<Components, ComponentRegister>::value)), ...);
  return archetype_id;
}

/////////////////////////////////////////////////
/// @brief Generates a vector of ArchetypeIDs, each corresponding to a single
/// Component
///
/// @tparam Components Derived types from Component.
/// @return A vector of ArchetypeIDs, each representing a single Component.
/////////////////////////////////////////////////
template <typename... Components>
std::vector<ArchetypeID> GenerateArchetypeIDsFromTypeList() {

  // create return vector
  std::vector<ArchetypeID> archetype_ids;

  // expand parameter pack and generate archetype ids for each single Component
  (archetype_ids.push_back(GenerateArchetypeIDfromTypes<Components>()), ...);
  return archetype_ids;
}

template <typename... Components>
std::set<size_t> GenerateEntityIndexesFromComponents(
    const std::unordered_map<ArchetypeID, Archetype> &archetypes,
    bool exact_match = true) {

  std::set<size_t> entity_indexes;

  // lamda to insert entity indexes from arcehtypes
  auto insert_entity_indexes = [&](const ArchetypeID &archetype_id) {};

  if (exact_match) {
    // generate archetype id from components with an exact match (there can only
    // be one)
    ArchetypeID target_id = GenerateArchetypeIDfromTypes<Components...>();

    // insert entity indexes from the matching archetype
    auto it = archetypes.find(target_id);
    if (it != archetypes.end()) {
      const Archetype &archetype = it->second;
      for (const size_t entity : archetype) {
        entity_indexes.insert(entity);
      }
    }
  } else {
    // generate archetype ids from each component type
    std::vector<ArchetypeID> target_ids =
        GenerateArchetypeIDsFromTypeList<Components...>();

    // use AND operaptor to compare ArchetypeIDs, anything >0 is a match
    for (const ArchetypeID &archetype_id : target_ids) {
      for (const auto &[key, value] : archetypes) {

        // check if any bits match
        if ((key & archetype_id).any()) {
          // insert entity indexes from the matching archetype
          const Archetype &archetype = value;
          for (const size_t entity : archetype) {
            entity_indexes.insert(entity);
          }
        }
      }
    }
  }
  return entity_indexes;
}

/////////////////////////////////////////////////
/// @brief Returns entity indexes sorted by m_priority of the given Component.
///
/// The Component type must have an integer m_priority member field.
/// Entities with equal priority retain their original index order.
///
/// @tparam Component Component type with an m_priority field.
/// @param archetypes Archetype map to search.
/// @param entity_memory_pool Pool used to read component priority values.
/// @param ascending When true, returns lowest priority first (for rendering).
///                  When false, returns highest priority first (for collision).
/// @return Sorted vector of entity indexes.
/////////////////////////////////////////////////
template <typename Component>
std::vector<size_t> GetEntitiesSortedByPriority(
    const std::unordered_map<ArchetypeID, Archetype> &archetypes,
    const EntityMemoryPool &entity_memory_pool, bool ascending = true) {

  // collect all matching entity indexes (exact match)
  std::set<size_t> entity_index_set =
      GenerateEntityIndexesFromComponents<Component>(archetypes, true);

  std::vector<size_t> entity_indexes(entity_index_set.begin(),
                                     entity_index_set.end());

  // sort by m_priority, using entity index as tie-breaker for stability
  std::stable_sort(
      entity_indexes.begin(), entity_indexes.end(),
      [&](size_t a, size_t b) {
        const Component &comp_a =
            entity::memory::GetComponent<Component>(a, entity_memory_pool);
        const Component &comp_b =
            entity::memory::GetComponent<Component>(b, entity_memory_pool);
        if (ascending) {
          return comp_a.m_priority < comp_b.m_priority;
        }
        return comp_a.m_priority > comp_b.m_priority;
      });

  return entity_indexes;
}
} // namespace steamrot::archetypes
