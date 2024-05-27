
#pragma once
#include "Entity.h"


class EntityManager
{

	std::vector<Entity> m_entities;
	std::vector<Entity> m_entitiesToAdd;

public:
	Entity	addEntity();
};