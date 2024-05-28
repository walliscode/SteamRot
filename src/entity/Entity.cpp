#include "Entity.h"

Entity::Entity(const std::string& tag, const size_t& id) : m_id(id), m_tag(tag)
{
	m_active = true; //set enity to active
}

bool Entity::isActive() const
{
	return m_active;
}

const std::string& Entity::tag() const
{
	return m_tag;
}

const size_t Entity::id() const
{
	return m_id;
}

void Entity::destroy()
{
	m_active = false;
}