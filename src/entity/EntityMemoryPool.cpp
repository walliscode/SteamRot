#pragma once
#include <vector>


#include "EntityMemoryPool.h"
#include "CTransform.h"
#include "Entity.h"

class EntityMemoryPool
{
public:
	Entity addEntity()
	{
		size_t index = getNextEntityIndex(); //get the next free index

		//reset all the componet data for the free index
		std::get<std::vector<CTransform>>(m_data)[index] = CTransform();

		//set all vectors[index] = 0
		//set active[index] = default
		return Entity(index);
	}

	int getNextEntityIndex() {
		return 0;
	}
};