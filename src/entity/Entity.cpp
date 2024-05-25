#include "Entity.h"

Entity::Entity(const size_t& id) :
	m_id(id)
{

}
Entity::Entity(const size_t& id, const ComponentTuple& entityConfig) :
	m_id(id),
	m_components(entityConfig)
{
}

bool Entity::isActive() const
{
	return m_active;
}

const size_t Entity::id() const
{
	return m_id;
}

void Entity::destroy()
{
	m_active = false;
}