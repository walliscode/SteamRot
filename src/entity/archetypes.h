#pragma once

#include "containers.h"
#include "entity_types.h"
#include <bitset>
#include <cstddef>

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
} // namespace steamrot::archetypes
