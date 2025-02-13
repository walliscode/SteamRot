#pragma once

#include "Archetype.h"
#include "ComponentCollections.h"
#include <cstdlib>

#include <vector>

class ArchetypeManager {
private:
  std::vector<Archetype> m_archetypes;

public:
  ArchetypeManager();

  // archetypes are a unique collection of Components. this is to aid in
  // filtering entities functions are provided to get arcehtypes based on and/or
  // type filtering
  //
  //
  // returns the archetype will contains all and only the provided Components
  const Archetype &
  getExactArchetype(const ComponentFlags &archetype_requirements) const;

  const std::vector<size_t> &
  getExactArchetypeEntities(const ComponentFlags &archetype_requirements) const;

  // get all archetypes that contain the provided Components
  const std::shared_ptr<std::vector<Archetype>>
  getInclusiveArchetype(const ComponentFlags &archetype_requirements) const;

  const std::shared_ptr<std::vector<size_t>> getInclusiveArchetypeEntities(
      const ComponentFlags &archetype_requirements) const;

  // void AssignEntityToArchetype(size_t entity_id,
  //                              std::vector<std::string> compTags);
  //
  // void ClearEntityFromArchetype(size_t entity_id,
  //                               std::vector<std::string> compTags);
  // void ReassessEntityArchetype(size_t entity_id,
  //                              std::vector<std::string> compTags);

  const std::vector<Archetype>
  getArchetypes() const; // return all the archetpes from the manager
};
