////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "containers.h"
#include <bitset>
#include <cstddef>
#include <cstdlib>
#include <unordered_map>
#include <vector>
namespace steamrot {
////////////////////////////////////////////////////////////
// typedefs
////////////////////////////////////////////////////////////
using Archetype = std::vector<size_t>;

using ArchetypeID =
    std::bitset<steamrot::components::containers::kComponentRegisterSize>;
////////////////////////////////////////////////////////////
// class ArchetypeManager
////////////////////////////////////////////////////////////
class ArchetypeManager {
private:
  ////////////////////////////////////////////////////////////
  // Member data
  ////////////////////////////////////////////////////////////
  std::unordered_map<ArchetypeID, Archetype> m_archetypes;

  /**
   * @brief Generates the ArchetypeID for a given entity based on its
   * Components.
   *
   * @param entity_memory_pool An instance of EntityMemoryPool
   * @param entity_index The index of the entity in the EntityMemoryPool
   * @return A bitset representing the ArchetypeID for the entity.
   */
  const ArchetypeID GenerateArchetypeID(
      const components::containers::EntityMemoryPool &entity_memory_pool,
      size_t entity_index);

public:
  ////////////////////////////////////////////////////////////
  /// \brief Default constructor
  ///
  ////////////////////////////////////////////////////////////
  ArchetypeManager();

  ////////////////////////////////////////////////////////////
  /// \brief returns an all entity indexes from provided ArchetypeIDs
  ///
  ////////////////////////////////////////////////////////////
  std::vector<size_t>
  GetEntityIndexes(const std::vector<ArchetypeID> &archtype_IDs) const;

  /**
   * @brief Creates all archetypes based on the current state of the entities.
   * To be run infrequently.
   *
   * @param entity_memory_pool A reference to the entity memory pool containing
   * all configurations for this scene.
   */
  void GenerateAllArchetypes(
      const components::containers::EntityMemoryPool &entity_memory_pool);

  /**
   * @brief Returns a reference to the map of Archetypes. This should not modify
   * the data
   *
   * @return archetype map
   */
  std::unordered_map<ArchetypeID, Archetype> &GetArchetypes();
};

} // namespace steamrot
