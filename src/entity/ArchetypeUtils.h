#pragma once

#include "containers.h"
#include <bitset>
#include <set>
#include <unordered_map>
#include <vector>

namespace steamrot {

using ArchetypeID = std::bitset<steamrot::kComponentRegisterSize>;
using Archetype = std::vector<size_t>;

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
/// @brief Generates a vector of ArchetypeIDs where all provided Components
/// are present in each ID (partial match).
///
/// This function iterates through all archetypes in the provided map and
/// returns those where all specified components are present, regardless of
/// what other components exist.
///
/// @tparam Components Parameter pack of types derived from Component.
/// @param archetypes The archetype map to search through.
/// @return A vector of ArchetypeIDs that contain all specified Components.
/////////////////////////////////////////////////
template <typename... Components>
std::vector<ArchetypeID> GenerateArchetypeIDsContainingComponents(
    const std::unordered_map<ArchetypeID, Archetype> &archetypes) {
  
  std::vector<ArchetypeID> matching_ids;
  
  // Generate the required component mask
  ArchetypeID required_mask;
  ((required_mask.set(IndexOf<Components, ComponentRegister>::value)), ...);
  
  // Iterate through all archetypes
  for (const auto &[archetype_id, archetype] : archetypes) {
    // Check if this archetype contains all required components
    // (archetype_id & required_mask) == required_mask means all bits in
    // required_mask are also set in archetype_id
    if ((archetype_id & required_mask) == required_mask) {
      matching_ids.push_back(archetype_id);
    }
  }
  
  return matching_ids;
}

/////////////////////////////////////////////////
/// @brief Gathers unique entity indices from archetypes based on component
/// matching strategy.
///
/// This function provides two strategies for gathering entities:
/// - Exact match: Only entities with exactly the specified components
/// - Partial match: All entities that have at least the specified components
///
/// @tparam Components Parameter pack of types derived from Component.
/// @param archetypes The archetype map to search through.
/// @param exact_match If true, only exact component matches are returned.
///                    If false, all archetypes containing the components are
///                    included.
/// @return An ordered set of unique entity indices.
/////////////////////////////////////////////////
template <typename... Components>
std::set<size_t> GatherEntityIndices(
    const std::unordered_map<ArchetypeID, Archetype> &archetypes,
    bool exact_match = true) {
  
  std::set<size_t> entity_indices;
  std::vector<ArchetypeID> archetype_ids;
  
  if (exact_match) {
    // Generate exact archetype ID
    ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<Components...>();
    archetype_ids.push_back(archetype_id);
  } else {
    // Generate all archetype IDs containing the specified components
    archetype_ids = GenerateArchetypeIDsContainingComponents<Components...>(archetypes);
  }
  
  // Gather all unique entity indices from the matching archetypes
  for (const auto &archetype_id : archetype_ids) {
    auto it = archetypes.find(archetype_id);
    if (it != archetypes.end()) {
      const Archetype &archetype = it->second;
      // Insert into set ensures uniqueness and ordering
      entity_indices.insert(archetype.begin(), archetype.end());
    }
  }
  
  return entity_indices;
}

} // namespace steamrot
