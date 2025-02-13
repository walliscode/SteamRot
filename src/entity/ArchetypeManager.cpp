#include "ArchetypeManager.h"
#include <cstddef>
#include <magic_enum/magic_enum.hpp>
#include <stdexcept>
#include <vector>


ArchetypeManager::ArchetypeManager() {}

// exact archetype is one that contains exactly the required components
// no more, no less
const Archetype &ArchetypeManager::getExactArchetype(
    const SteamRot::ComponentFlags &archetype_finder) const {

  // here we can use the == operator to compare the component flags
  // this is because we want the exact combination of components
  // this will produce a unique size_t value for each combination of components
  for (auto &arch : m_archetypes) {
    if (arch.getID() == archetype_finder) {
      return arch;
    }
  }

  throw std::runtime_error("Archetype not found");
};

const std::vector<size_t> &ArchetypeManager::getExactArchetypeEntities(
    const SteamRot::ComponentFlags &archetype_finder) const {

  return getExactArchetype(archetype_finder).getEntities();
};

// Inclusive archetypes are those that contain at least the required components
const std::shared_ptr<std::vector<Archetype>>
ArchetypeManager::getInclusiveArchetype(
    const SteamRot::ComponentFlags &archetype_requirements) const {
  std::vector<Archetype> return_set;

  // Loop through all archetypes adding each code that contains at least the tag
  // code

  for (auto &arch : m_archetypes) {
    // use & bit wise operator from magic enum to get inclusive archetypes
    // this will return a non-zero value if the archetype contains the required
    // components
    using namespace magic_enum::bitwise_operators;
    int flag_check = static_cast<int>(arch.getID() & archetype_requirements);
    if (flag_check != 0) {
      return_set.push_back(arch);
    }
  }
  return std::make_shared<std::vector<Archetype>>(return_set);
}

const std::shared_ptr<std::vector<size_t>>
ArchetypeManager::getInclusiveArchetypeEntities(
    const SteamRot::ComponentFlags &archetype_requirements) const {

  // create a vector to store all entities for all matching archetypes' entities
  std::vector<size_t> entities_set;
  std::vector<Archetype> archetype_set =
      *getInclusiveArchetype(archetype_requirements);

  // loop through all archetypes and add all entities to the set
  // this currently does not care about duplicates
  for (auto &arch : archetype_set) {
    entities_set.insert(entities_set.end(), arch.getEntities().begin(),
                        arch.getEntities().end());
  }
  return std::make_shared<std::vector<size_t>>(entities_set);
}

// void ArchetypeManager::AssignEntityToArchetype(
//     size_t assEntity, std::vector<std::string> compTags) {
//   // Gen the archcode for the given entity
//   std::unique_ptr<size_t> entCode = generateTagCode(compTags);
//   // loop through the arch lists and add this entity to the correct list
//
//   for (auto &arch : m_archetypes) {
//     if (*arch.getCode() == *entCode) {
//       arch.addEntity(assEntity);
//       return;
//     }
//   }
//
//   // if no archetype existed with the correct code, create a new archetype
//   std::vector<size_t> newIDSet;
//   for (auto &tag : compTags) {
//     newIDSet.push_back(compTagMap[tag]);
//   }
//   m_archetypes.push_back(*new Archetype(newIDSet));
//   m_archetypes.back().addEntity(assEntity);
// }
//
// void ArchetypeManager::clearEntity(size_t clrEntity,
//                                    std::vector<std::string> compTags) {
//   // Gen the archcode for the given entity
//   std::unique_ptr<size_t> entCode = generateTagCode(compTags);
//
//   // loop through the arch lists and remove this entity from the correct list
//   for (auto &arch : m_archetypes) {
//     if (*arch.getCode() == *entCode) {
//       arch.removeEntity(clrEntity);
//       break;
//     }
//   }
// }
//
// void ArchetypeManager::reassessEntity(size_t reAssEntity,
//                                       std::vector<std::string> compTags) {
//   // find the old archetype and remove this entity
//   for (auto &arch : m_archetypes) {
//     if (arch.contains(reAssEntity)) {
//       arch.removeEntity(reAssEntity);
//       break;
//     }
//   }
//
//   // add the entity to the correct list
//   assignArchetype(reAssEntity, compTags);
// }

const std::vector<Archetype> ArchetypeManager::getArchetypes() const {
  return m_archetypes;
}
