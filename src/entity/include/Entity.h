// class for the entity
#pragma once
#include <tuple>
#include "Component.h"



class Entity
{	


	bool				m_active = true;      // is the entity active or not, helps with entity destruction. NOT a component
	const size_t        m_id = 0;     // unique id for the entity  
	ComponentTuple		m_components;         // tuple of components that the entity has


	Entity(const size_t& id, const ComponentTuple& entityConfig);
public:

	Entity(const size_t& id);

	// private member access functions
	bool                  isActive() const;
	const size_t          id() const;
	void                  destroy();

	template <typename T>
	bool hasComponent() const
	{
		return getComponent<T>().has;
	}

	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... mArgs)
	{
		auto& component = getComponent<T>();
		component = T(std::forward<TArgs>(mArgs)...);
		component.has = true;
		return component;
	}

	template <typename T>
	T& getComponent()
	{
		return std::get<T>(m_components);
	}

	template <typename T>
	const T& getComponent() const
	{
		return std::get<T>(m_components);
	}

	template <typename T>
	void removeComponent()
	{
		getComponent<T>() = T();
	}

};