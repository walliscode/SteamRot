#pragma once
#include "EntityManager.h"


EntityManager::EntityManager(int poolSize) : m_pool(new EntityMemoryPool(poolSize))
{
}

size_t EntityManager::addEntity()
{
	size_t newEntityID = (*m_pool).getNextEntityIndex(); //get the next free index in the memory pool, set to active and return the index
	(*m_pool).refreshEntity(*(*m_pool).getData(), newEntityID); //call the refresh entity function on the new ID to clear it
	std::get<0>(*(*m_pool).getData())[newEntityID] = true; //set the active vector to true at the new entity index
	return newEntityID; //return the index of the new entity
}

void EntityManager::removeEntity(size_t entityID)
{
	std::get<0>(*(*m_pool).getData())[entityID] = false; //set the active vector to false at the passed entity index
}