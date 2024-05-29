#pragma once
#include "EntityManager.h"


EntityManager::EntityManager(int poolSize) : m_pool(new EntityMemoryPool(poolSize))
{
}

int EntityManager::addEntity()
{
	int newEntityID = (*m_pool).getNextEntityIndex(); //get the next free index in the memory pool, set to active and return the index
	std::get<0>(*(*m_pool).getData())[newEntityID] = true; //set the active vector to true at the new entity index
	(*m_pool).refreshEntity(*(*m_pool).getData(), newEntityID); //call the refresh entity function on the new ID to clear it
	return newEntityID; //return the index of the new entity
}

void EntityManager::removeEntity(int entityID)
{
	std::get<0>(*(*m_pool).getData())[entityID] = false; //set the active vector to false at the passed entity index
}