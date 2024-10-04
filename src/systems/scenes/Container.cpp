#include "Container.h"


Container::Container(const std::string& tag)
	: m_tag(tag)
{
		
}


std::string Container::getTag() const
{
	return m_tag;
}

void Container::setTag(const std::string& tag)
{
	m_tag = tag;
}

std::vector<size_t> Container::getEntityIndexes() const
{
	return m_entityIndexes;
}

void Container::addEntityIndex(size_t index)
{
	m_entityIndexes.push_back(index);
}


sf::RectangleShape& Container::getShape()
{
	return m_shape;
}

sf::Vector2f Container::getProportionalSize() const
{
	return m_prortionalSize;
}

void Container::setProportionalSize(const sf::Vector2f& size)
{
	m_prortionalSize = size;
}

std::vector<std::shared_ptr<Container>> Container::getChildrenContainers() const
{
	return m_childrenContainers;
}

void Container::addChildContainer(std::shared_ptr<Container> container)
{
	m_childrenContainers.push_back(container);
}



