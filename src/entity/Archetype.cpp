#include "Archetype.h"
#include "ComponentCollections.h"
#include <algorithm>

Archetype::Archetype(const ComponentFlags &archetype_components)
    : m_id(archetype_components) {};

const ComponentFlags &Archetype::getID() const { return m_id; }

const std::vector<size_t> &Archetype::getEntities() const { return m_entities; }

void Archetype::AddEntityToArchetype(size_t entity_id) {
  m_entities.push_back(entity_id);
}

void Archetype::RemoveEntityFromArchetype(size_t entity_id) {
  m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), entity_id),
                   m_entities.end());
}

const bool Archetype::ContainsEntity(size_t entity) const {
  return std::find(m_entities.begin(), m_entities.end(), entity) !=
         m_entities.end();
}
