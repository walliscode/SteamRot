#pragma once

#include "ComponentFlags.h"
#include <cstdlib>
#include <vector>

class Archetype {
private:
  std::vector<size_t> m_entities;
  SteamRot::ComponentFlags m_id;

public:
  Archetype(const SteamRot::ComponentFlags &archetype_components);

  const SteamRot::ComponentFlags &getID() const;
  const std::vector<size_t> &getEntities() const;
  void AddEntityToArchetype(size_t entity_id);
  void RemoveEntityFromArchetype(size_t entity_id);

  const bool ContainsEntity(size_t entity) const;
};
