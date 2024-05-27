#pragma once
#include <tuple> 
#include <vector>

#include "EntityManager.h"
#include "CTransform.h"

typedef std::tuple<
	std::vector<bool>,	//bool to track if pool position is active
	std::vector<CTransform>
> EntityComponentVectorTuple;

class EntityMemoryPool
{
	size_t						m_numEntities; //total number of entitys
	EntityComponentVectorTuple  m_data; //store of component vectors (all component data for this pool)
	std::vector<bool>			m_active; //is this column of the component vectors referencing a live entity
	EntityMemoryPool(size_t maxEntities); //constructor for the memory pool

public:

	//Entity pool functions
	Entity addEntity();	//add a new entity to the list
	int getNextEntityIndex(); //return the next free index for a new entity


	//Entity pool templates

	template <typename T>
	T& getComponent(size_t entityID) {
		return std::get<std::vector<T>>(m_data)[entityID]; //return the component of the passed component type for the requested entityID
	}

	template <typename T>
	T& hasComponent(size_t entityID) {
		return std::get<std::vector<T>>(m_data)[entityID].has(); //return the component of the passed component type for the requested entityID
	}
};