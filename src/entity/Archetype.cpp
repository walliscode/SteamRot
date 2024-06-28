#pragma once
#include "Archetype.h"

Archetype::Archetype(std::vector<size_t> idSet) : m_idSet(idSet) {}

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
	m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), remEntity), m_entities.end());
}

const size_t& Archetype::getCode() const {
	size_t archCode = 0;
	for (auto& compID : m_idSet) {
		archCode = archCode | (1 << (compID - 1)); //for each ID, OR the current code with 1, bitshifted by the ID - 1 (add a 1 to the code at the binary position set by the ID)
	};
	return archCode;
}