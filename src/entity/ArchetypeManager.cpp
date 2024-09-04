#pragma once
#include "ArchetypeManager.h"

ArchetypeManager::ArchetypeManager() {}

const Archetype& ArchetypeManager::getArchetype() const {
	//TODO return the archetype for the given component set
	return m_archetypes[0]; 
}

const std::vector<size_t>& ArchetypeManager::getArchetypeEntities() const {
	return getArchetype().getEntities(); //return the entities for the given component set
}

void ArchetypeManager::assignArchetype(size_t assEntity) {
	//TODO add the entity ID to the correct archetype based on the componets
}

void ArchetypeManager::clearEntity(size_t clrEntity) {
	//TODO remove the entity ID from it's archtype list
}

void ArchetypeManager::reassessEntity(size_t reAssEntity) {
	//TODO Re-assess the entity ID and move it to the correct archetype if it's in the wrong one
}