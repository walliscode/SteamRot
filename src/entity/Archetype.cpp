#pragma once
#include "Archetype.h"

Archetype::Archetype(std::vector<size_t> idSet) : m_id(idSet) {}

const std::vector<size_t>& Archetype::getIDSet() const {
	return m_idSet;
}

const std::vector<size_t>& Archetype::getEntities() const {
	return m_entities;
}

void Archetype::addEntity(size_t newEntity) {
	m_entities.push_back(newEntity);
}

void Archetype::removeEntity(size_t remEntity) {
	m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), remEntity, m_entities.end());
}